#pragma once
#include <cstddef>

template <typename T, T UnitElement = 1>
T BinPow(T base, std::size_t exponent) {
  T result = UnitElement;
  while(exponent ) {
    if(exponent & 1) result *= base;
    base *= base;
    exponent /= 2;
  }

  return result;
}
