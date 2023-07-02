#include "primes.hpp"
#include <iostream>
#include <algorithm>


int main() {
  const int n = 1'000'000'000;  
  auto& primes = get_primes_until_n(n);
  // find the last pos where primes[pos] <= n
  int pos = std::lower_bound(primes.begin(), primes.end(), n) - primes.begin();
  std::cout << pos << '\n';


  return 0;
}