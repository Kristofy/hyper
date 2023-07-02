#pragma once

/** The algorithm goes like this:
 *  1. Take the constant data set
 *  2. Hash the date set's elements
 *  3. Use the hash as an index into a table, the table initialli has a size of 2^k where k is ceil(log2(n))
 *  4. If a collision occurs, tweek the hash function
 *  5. Repeat until no collisions
 *  6. If there is no hash function for the current size of the table, resize the table to be double the size 
 *  7. When we have a solution than the hash function and the table represents the phf
 * 
 *  The system should work with a permuting hash function, but it is not necessary
 * 
 *  How to tweek the hash function:
 *  - XOR the hash with a constant
 *
 * 
**/

#include "phf.hpp"

#include <cstdint>
#include <functional> // std::hash
#include <stdexcept> // std::bad_alloc

namespace hyper::phf {
// work in progress


// template <typename KeyType, typename ValueType>
// class phf_map {
//   public:

//   phf_map(const KeyType *const keys, const ValueType* const vals, std::size_t size)
//     : values(vals), 
//       size(size), 
//       hash_table_size_log2(32 - __builtin_clz(size - 1) + 1) {
      
//       hash_table = new ValueType*[1 << hash_table_size_log2];
//       if(hash_table == nullptr) { throw std::bad_alloc(); }

//       bool had_collision = false;
//       do {
//         had_collision = false;

//         for(std::size_t i = 0; i < size && !had_collision; ++i) {
//           std::size_t hash = std::hash(keys[i]);
//           std::size_t index = hash & ((1 << hash_table_size_log2) - 1);

//           had_collision = hash_table[index];
//           hash_table[index] = vals[i];
//         }

//         if(had_collision) {
//           hash_tweek += 1;
//         }


//       } while (had_collision);



//     }

//   ~phf_map() {
//     delete[] hash_table;
//   }

//   private:
//   ValueType* hash_table;
//   std::size_t hash_table_size_log2;
//   std::size_t hash_tweek = 1;
//   int size;
// }


} // hyper::phf