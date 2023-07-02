#pragma once

#include <cstdint>

namespace hyper::rng {

constexpr inline uint32_t pcg_hash(uint32_t input) noexcept {
  uint32_t state = input * 747796405U + 2891336453U;
  uint32_t word = ((state >> ((state >> 28U) + 4U)) ^ state) * 277803737U;
  return (word >> 22U) ^ word;
}

constexpr inline uint64_t pcg_rng(uint64_t& state) noexcept {
  return state = pcg_hash(state);
}

constexpr inline uint64_t pcg_hash(uint64_t state) noexcept {
  uint64_t word = ((state >> ((state >> 59U) + 5U)) ^ state) * 12605985483714917081ULL;
  return (word >> 43U) ^ word;
}

constexpr inline uint64_t pcg_rng(uint64_t& state) noexcept {
  return state = pcg_hash(state);
}

} // hyper::rng