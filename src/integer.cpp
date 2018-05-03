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
      } else if (lhs.digits[i] < rhs.digits[i]) {
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
  // Only implemented for positive integers, where the result is positive.
  operator+=(-rhs);
  return *this;
}

Int& Int::operator*=(const Int& rhs) {
  // Only implemented for positive integers, where the result is positive.
  Int orig = *this;
  for (int i = 1; i < rhs; i += 1) {
    add_ignoring_sign(orig);
  }
  is_negative ^= rhs.is_negative;
  return *this;
}

Int& Int::operator/=(const Int& rhs) {
  // Only implemented for positive integers, where the result is positive.
  Int orig = *this;
  Int res = 0;
  while (orig > rhs) {
    orig -= rhs;
    res += 1;
  }
  *this = res;
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

bool sum_is_safe(uint32_t x, uint32_t y) {
  return y <= std::numeric_limits<uint32_t>::max() - x;
}

std::pair<uint32_t, uint32_t> add_with_carry(uint32_t x, uint32_t y,
                                             uint32_t carry) {
  assert(carry == 0 || carry == 1);
  // sum_overflows is true iff x + y + carry > 2^64 - 1.
  bool sum_overflows = !sum_is_safe(x, y) || !sum_is_safe(x + y, carry);
  return {x + y + carry, sum_overflows ? 1 : 0};
}
