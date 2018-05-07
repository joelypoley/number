#ifndef NUMBER_SRC_INTEGER_H
#define NUMBER_SRC_INTEGER_H

#include <algorithm>
#include <cstdint>
#include <ostream>
#include <string>
#include <vector>

class Int {
 public:
  Int(int32_t a);
  Int(std::string a);
  friend bool operator<(const Int& lhs, const Int& rhs);
  friend bool operator==(const Int& lhs, const Int& rhs);
  friend bool less_in_magnitude(const Int& lhs, const Int& rhs);
  Int& operator+=(const Int& rhs);
  Int& operator-=(const Int& rhs);
  Int& operator*=(const Int& rhs);
  Int& operator/=(const Int& rhs);
  Int operator-() const;
  int sign() const { return is_negative ? -1 : 1; }
  std::vector<uint32_t> get_digits() const { return digits; }
  std::string debug_string() const;
  void shift_by(int i);
  Int mod(const Int& rhs) const;
 private:
  // True if integer is strictly less than 0.
  bool is_negative;

  // Integer is stored in base 2^32 where each digit is an element of the vector
  // v. v[0] is the least significant digit of the integer.
  std::vector<uint32_t> digits;

  // Implements the borrowing method used in the subtraction algorithm you learn
  // in primary school. The input is the index of the digit to borrow from.
  void borrow_from_neighbour(int neighbour);

  void add_ignoring_sign(const Int& rhs);
  void subtract_ignoring_sign(const Int& rhs);
  void remove_leading_zeros();
  Int multiply_ignoring_sign(Int x, uint32_t y);
  void divide_ignoring_sign(const Int& rhs);
  void divide_by_2();
};

bool sum_is_safe(uint32_t x, uint32_t y);

std::pair<uint32_t, uint32_t> add_with_carry(uint32_t x, uint32_t y,
                                             uint32_t carry);

std::pair<uint32_t, uint32_t> multiply_with_carry(uint32_t x, uint32_t y,
                                                  uint32_t carry);

bool operator!=(const Int& lhs, const Int& rhs) {
  return !operator==(lhs, rhs);
}

bool operator>(const Int& lhs, const Int& rhs) { return operator<(rhs, lhs); }

bool operator<=(const Int& lhs, const Int& rhs) { return !operator>(lhs, rhs); }

bool operator>=(const Int& lhs, const Int& rhs) { return !operator<(lhs, rhs); }

Int operator+(Int lhs, const Int& rhs) { return lhs += rhs; }
Int operator-(Int lhs, const Int& rhs) { return lhs -= rhs; }
Int operator*(Int lhs, const Int& rhs) { return lhs *= rhs; }
Int operator/(Int lhs, const Int& rhs) { return lhs /= rhs; }

void PrintTo(const Int& a, std::ostream* os) {
  *os << a.debug_string();  // whatever needed to print bar to os
}

#endif  // NUMBER_SRC_INTEGER_H
