#pragma once

#include <cmath>
#include <vector>
#include <cstring>
#include <cstdint>

#include <bitset>

template <typename IntegerType = unsigned long long, typename FloatingPointType = long double>
inline constexpr IntegerType upper_bound_for_primes_till_n(IntegerType n) noexcept {
  return n / (std::log(n) - 1.1);
}

template <typename IntegerType = unsigned long long>
const std::vector<IntegerType>& get_primes_until_n(IntegerType n) {
  static std::vector<IntegerType> primes = {
    2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 
    71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 
    157, 163, 167, 173, 179, 181, 191, 193, 197, 199, 211, 223, 227, 229, 233, 239, 
    241, 251, 257, 263, 269, 271, 277, 281, 283, 293, 307, 311, 313, 317, 331, 337, 
    347, 349, 353, 359, 367, 373, 379, 383, 389, 397, 401, 409, 419, 421, 431, 433, 
    439, 443, 449, 457, 461, 463, 467, 479, 487, 491, 499, 503, 509, 521, 523, 541, 
    547, 557, 563, 569, 571, 577, 587, 593, 599, 601, 607, 613, 617, 619, 631, 641, 
    643, 647, 653, 659, 661, 673, 677, 683, 691, 701, 709, 719, 727, 733, 739, 743, 
    751, 757, 761, 769, 773, 787, 797, 809, 811, 821, 823, 827, 829, 839, 853, 857, 
    859, 863, 877, 881, 883, 887, 907, 911, 919, 929, 937, 941, 947, 953, 967, 971, 
    977, 983, 991, 997, 1009, 1013, 1019, 1021
  };

  static IntegerType last_extended_n = 0;
  static IntegerType last_checked_value = primes.back();
  if(n <= last_checked_value) { return primes; }

  primes.reserve(upper_bound_for_primes_till_n(n+1));
  std::vector<int64_t> is_prime;

  while(n > last_checked_value) {

    const IntegerType next_extended_n = last_extended_n + 128;
    const IntegerType last_extended_n_prime = primes[last_extended_n];
    const IntegerType next_extended_n_prime = primes[next_extended_n];

    const IntegerType lower = last_extended_n_prime * last_extended_n_prime;
    const IntegerType upper = std::min(n+1, next_extended_n_prime * next_extended_n_prime - 1);
    const IntegerType max_primes_in_intervall = upper - lower + 1;

    is_prime.resize(max_primes_in_intervall / 64 + 1);
    memset(is_prime.data(), 0, is_prime.size() * sizeof(uint64_t));

    // the first position in primes what lower + prime > upper
    int pos = std::lower_bound(primes.begin(), primes.end(), upper - lower) - primes.begin();

    #pragma omp parallel for
    for(int i = 0; i < pos; i++) {
      IntegerType prime = primes[i];
      IntegerType lower_bound = ((lower + prime - 1) / prime) * prime;

      for(IntegerType i = lower_bound - lower; i < max_primes_in_intervall; i += prime) {
        #pragma omp atomic write
        is_prime[i / 64] |= 1ULL << (i & 63);
      }
    }

    for(int i = 0; i < max_primes_in_intervall / 64; i++) {
      auto mask = ~is_prime[i]; 
      while(mask) {
        int j = __builtin_ctzll(mask);  // Get index of least significant set bit
        primes.push_back(i * 64 + j + lower);
        mask &= (mask - 1);  // Clear least significant set bit
      }
    }

    int i = max_primes_in_intervall / 64;
    auto mask = (~is_prime[i]) & ((1ULL << ((max_primes_in_intervall - i * 64) + 1))-1); 
    while(mask) {
      int j = __builtin_ctzll(mask);  // Get index of least significant set bit
      primes.push_back(i * 64 + j + lower);
      mask &= (mask - 1);  // Clear least significant set bit
    }
    // only interested in the first max_primes_in_intervall - i * 64 bits
    // so mask for it
    // for(int j = 0; j < max_primes_in_intervall - i * 64; j++) {
    //   if( mask & (1ULL << j) ) {
    //     primes.push_back(i*64 + j + lower);
    //   }
    // }

    last_checked_value = upper - 1;
    last_extended_n = next_extended_n;
  }

  return primes;
}