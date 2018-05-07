#include "integer.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdint>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>

namespace {
template <typename T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& v) {
  if (!v.empty()) {
    out << '[';
    std::copy(v.begin(), v.end(), std::ostream_iterator<T>(out, ", "));
    out << "\b\b]";
  }
  return out;
}
}  // namespace

Int::Int(int32_t a) {
  static_assert(
      static_cast<uint32_t>(-1) == static_cast<uint32_t>(0xFFFFFFFFULL),
      "Your compiler must use two's complement.");

  if (a == std::numeric_limits<int32_t>::min()) {  // Deal with special case.
    is_negative = true;
    digits.push_back(0x80000000ULL);
    return;
  }

  is_negative = a < 0;
  int32_t abs_a = is_negative ? -a : a;
  digits.push_back(abs_a);
}

Int::Int(std::string a) {
  is_negative = false;
  if (a.empty()) {
    throw std::invalid_argument("string must be nonempty");
  }
  if (a == "-0") {
    is_negative = false;
    digits.push_back(0);
    return;
  }

  auto numeric_start_it = a.begin();
  if (a[0] == '-') {
    ++numeric_start_it;
  }
  digits.push_back(0);
  for (auto it = numeric_start_it; it != a.end(); ++it) {
    char c = *it;
    if (!isdigit(c)) {
      throw std::invalid_argument("string must be numeric");
    }
    *this *= Int(10);
    *this += Int(c - '0');
  }
  is_negative = (a[0] == '-');
}

bool operator<(const Int& lhs, const Int& rhs) {
  if (rhs.is_negative && !lhs.is_negative) {
    return false;
  }
  if (!rhs.is_negative && lhs.is_negative) {
    return true;
  }
  // From this point on they are either both negative or both nonnegative.
  bool both_negative = lhs.is_negative;
  if (both_negative) {
    return !less_in_magnitude(lhs, rhs);
  } else {
    return less_in_magnitude(lhs, rhs);
  }
}

bool operator==(const Int& lhs, const Int& rhs) {
  return (lhs.digits == rhs.digits) && (lhs.is_negative == rhs.is_negative);
}

bool less_in_magnitude(const Int& lhs, const Int& rhs) {
  if (lhs.digits.size() < rhs.digits.size()) {
    return true;
  } else if (lhs.digits.size() > rhs.digits.size()) {
    return false;
  } else {
    for (int i = lhs.digits.size() - 1; i >= 0; --i) {
      if (lhs.digits[i] < rhs.digits[i]) {
        return true;
      } else if (lhs.digits[i] > rhs.digits[i]) {
        return false;
      }
    }
    return false;
  }
}

Int& Int::operator+=(const Int& rhs) {
  if (*this >= 0 && rhs >= 0) {
    add_ignoring_sign(rhs);
  } else if (*this < 0 && rhs < 0) {
    add_ignoring_sign(rhs);
    is_negative = true;
  } else if (!less_in_magnitude(*this, rhs)) {
    subtract_ignoring_sign(rhs);
  } else {
    Int temp = *this;
    *this = rhs;
    subtract_ignoring_sign(temp);
  }
  if (digits.size() == 1 && digits[0] == 0) {
    is_negative = false;
  }
  return *this;
}

Int& Int::operator-=(const Int& rhs) {
  operator+=(-rhs);
  return *this;
}

Int& Int::operator*=(const Int& rhs) {
  Int result = 0;
  for (int i = 0; i < rhs.digits.size(); ++i) {
    Int single_multiplication = multiply_ignoring_sign(*this, rhs.digits[i]);
    single_multiplication.shift_by(i);
    result += single_multiplication;
  }
  result.is_negative = is_negative;
  *this = result;
  is_negative ^= rhs.is_negative;
  if (digits.size() == 1 && digits[0] == 0) {
    is_negative = false;
  }
  return *this;
}

Int& Int::operator/=(const Int& rhs) {
  // Only implemented for positive integers, where the result is positive.
  Int internal_rhs = rhs;
  const bool this_is_negative = is_negative;
  const bool rhs_is_negative = internal_rhs.is_negative;
  is_negative = false;
  internal_rhs.is_negative = false;
  divide_ignoring_sign(internal_rhs);
  is_negative = this_is_negative ^ rhs_is_negative;
  return *this;
}

Int Int::operator-() const {
  if (*this == 0) {
    return *this;
  }
  Int res = *this;
  res.is_negative = !is_negative;
  return res;
}

std::string Int::debug_string() const {
  std::ostringstream out;
  out << (sign() == 1 ? "+" : "-") << digits;
  return out.str();
}

void Int::borrow_from_neighbour(int neighbour) {
  assert(neighbour < digits.size());
  while (digits[neighbour] == 0) {
    digits[neighbour] = std::numeric_limits<uint32_t>::max();
    ++neighbour;
    assert(neighbour < digits.size());
  }
  --digits[neighbour];
}

void Int::add_ignoring_sign(const Int& rhs) {
  uint32_t carry = 0;
  int i = 0;
  while (i < std::max(digits.size(), rhs.digits.size()) || carry != 0) {
    // First ensure that there is a place to store the next digit.
    if (i >= digits.size()) {
      digits.push_back(0);
      assert(i == digits.size() - 1);
    }

    std::tie(digits[i], carry) = add_with_carry(
        digits[i], i < rhs.digits.size() ? rhs.digits[i] : 0, carry);
    ++i;
  }
}

void Int::subtract_ignoring_sign(const Int& rhs) {
  assert(!less_in_magnitude(*this, rhs));
  for (int i = 0; i < digits.size(); ++i) {
    uint32_t curr_rhs_digit = i < rhs.digits.size() ? rhs.digits[i] : 0;
    if (digits[i] < curr_rhs_digit) {
      borrow_from_neighbour(i + 1);
    }
    digits[i] -= curr_rhs_digit;
  }
  remove_leading_zeros();
}

void Int::remove_leading_zeros() {
  while (digits.size() > 0 && digits.back() == 0) {
    digits.pop_back();
  }
  if (digits.empty()) {
    digits.push_back(0);
  }
}

Int Int::multiply_ignoring_sign(Int x, const uint32_t y) {
  x.is_negative = false;
  uint32_t carry = 0;
  for (int i = 0; i < x.digits.size(); ++i) {
    std::tie(x.digits[i], carry) = multiply_with_carry(x.digits[i], y, carry);
  }
  if (carry != 0) {
    x.digits.push_back(carry);
  }
  return x;
}

void Int::divide_ignoring_sign(const Int& rhs) {
  // Assumes that *this is nonegative and and rhs is positive.
  assert(rhs != 0);
  if (*this < rhs) {
    *this = 0;
    return;
  }
  Int lo{0};
  Int hi{1};
  hi.shift_by(digits.size() - rhs.digits.size() + 1);
  Int ans{0};
  int i = 0;
  while (lo <= hi) {
    ++i;
    Int mid = lo + hi;
    mid.divide_by_2();
    if (mid * rhs == *this) {
      *this = mid;
      return;
    } else if (*this < mid * rhs) {
      hi = mid - 1;
    } else if (*this > mid * rhs) {
      lo = mid + 1;
      ans = mid;
    }
  }
  *this = ans;
}

void Int::divide_by_2() {
  assert(digits.size() != 0);
  if (digits.size() == 1) {
    digits[0] >>= 1;
    return;
  }
  for (int i = 1; i < digits.size(); ++i) {
    digits[i - 1] >>= 1;
    uint32_t lsb = digits[i] & 1U;
    digits[i - 1] |= (lsb << 31);
  }
  digits[digits.size() - 1] >>= 1;
  remove_leading_zeros();
}

// Multiply by (2^32)^i.
void Int::shift_by(int i) {
  assert(i >= 0);
  if (*this == 0) {
    return;
  }

  for (int j = 0; j < i; ++j) {
    digits.push_back(0);
  }

  std::rotate(digits.rbegin(), digits.rbegin() + i, digits.rend());
}

Int Int::mod(const Int& rhs) const {
  assert(rhs > 0);
  return *this - rhs * (*this / rhs);
}

Int& Int::reduce_mod(const Int& rhs) {
  *this = mod(rhs);
  return *this;
}

std::string Int::print() const {
  // Currently only workd for positive integers.
  if (*this == 0) {
    return "0";
  }
  std::string result;
  Int this_copy = *this;
  this_copy.is_negative = false;
  while (this_copy != 0) {
    Int next_digit = this_copy.mod(10);
    for (int i = 0; i < 10; ++i) {
      if (i == next_digit) {
        result += std::to_string(i);
      }
    }
    this_copy /= 10;
  }
  if (is_negative) {
    result += "-";
  }
  std::reverse(result.begin(), result.end());
  return result;
}

bool sum_is_safe(uint32_t x, uint32_t y) {
  return y <= std::numeric_limits<uint32_t>::max() - x;
}

std::pair<uint32_t, uint32_t> add_with_carry(uint32_t x, uint32_t y,
                                             uint32_t carry) {
  assert(carry == 0 || carry == 1);
  const uint64_t big_x = x;
  const uint64_t big_y = y;
  const uint64_t big_carry = carry;
  const uint64_t result = big_x + big_y + big_carry;
  const uint32_t sum = static_cast<uint32_t>(result & 0xFFFFFFFFULL);
  const uint32_t result_carry = static_cast<uint32_t>(result >> 32);

  return {sum, result_carry};
}

std::pair<uint32_t, uint32_t> multiply_with_carry(const uint32_t x,
                                                  const uint32_t y,
                                                  const uint32_t carry) {
  const uint64_t big_x = x;
  const uint64_t big_y = y;
  const uint64_t big_carry = carry;
  const uint64_t result = big_x * big_y + big_carry;
  const uint32_t product = static_cast<uint32_t>(result & 0xFFFFFFFFULL);
  const uint32_t result_carry = static_cast<uint32_t>(result >> 32);

  return {product, result_carry};
}
