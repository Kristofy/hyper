#pragma once
#include <algorithm>
#include <cassert>
#include <iostream>
#include <mutex>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <utility>
#include <vector>

/// @brief Meta programming library, containing compile time functions on types
namespace mpl {

/// @brief Returns the size of a tuple
/// @tparam Tuple
template<class Tuple>
struct tuple_size : public std::tuple_size<Tuple> {};

/// @brief Checks if a given type is a tuple
/// @tparam Tuple
template<class Tuple>
struct is_tuple : public std::false_type {};
template<class... Types>
struct is_tuple<std::tuple<Types...>> : public std::true_type {};


/// @brief Returns the first number i where get<i>(Tuple) is of type T
/// @tparam T
/// @tparam Tuple
template<class T, class Tuple>
struct tuple_index;

template<class T, class... Types>
struct tuple_index<T, std::tuple<T, Types...>> {
  static const std::size_t value = 0; // NOLINT
};

template<class T, class U, class... Types>
struct tuple_index<T, std::tuple<U, Types...>> {
  static const std::size_t value = 1 + tuple_index<T, std::tuple<Types...>>::value; // NOLINT
};

/// @brief Checks if a tuple includes a certain type
/// @tparam T
/// @tparam Tuple
template<typename T, typename Tuple>
struct has_type;

template<typename T>
struct has_type<T, std::tuple<>> : std::false_type {};

template<typename T, typename U, typename... Ts>
struct has_type<T, std::tuple<U, Ts...>> : has_type<T, std::tuple<Ts...>> {};

template<typename T, typename... Ts>
struct has_type<T, std::tuple<T, Ts...>> : std::true_type {};

/// @brief Evaluates a contex aware function of every type of a tuple
/// @tparam F The function type (will be deduced)
/// @tparam Ctx The type of the context (will be deduced)
/// @tparam Tuple The tuple type (Will be deduced)
/// @tparam I Should not be set, it's set automatically
/// @param ctx the context
template<typename F, typename Ctx, typename Tuple, std::size_t I = std::tuple_size_v<std::remove_reference_t<std::decay_t<Tuple>>>>
void ForEachInTuple(Ctx& ctx) {
  if constexpr (I > 0) {
    F lambda(ctx);
    lambda.template operator()<std::tuple_element_t<I - 1, std::remove_reference_t<std::decay_t<Tuple>>>>();
    ForEachInTuple<F, Ctx, Tuple, I - 1>(ctx);
  }
}

template<typename Tuple, typename Fn>
constexpr bool all_tuple(Fn&& pred) {
  return [&]<typename... T>(std::type_identity<std::tuple<T...>>) {
    return (pred(std::type_identity<T>{}) && ...);
  }(std::type_identity<Tuple>{});
}

template<typename ...Types>
constexpr std::tuple<Types& ...> tie(Types&& ...args) {
  return std::tuple<Types& ...>(args ...);
}


template <typename Tuple>
struct tuple_of_references {
  static constexpr bool value = mpl::all_tuple<Tuple>(
      [](auto type){ return std::is_reference_v<typename decltype(type)::type>;});
};


template <typename Tuple>
struct tuple_of_non_const {
  static constexpr bool value = mpl::all_tuple<Tuple>(
      [](auto type){ return !std::is_const_v<typename decltype(type)::type>;});
};

// Base template
template <template <typename...> class, typename>
struct is_specialization_of : std::false_type {};

// Specialized template
template <template <typename...> class Template, typename... Args>
struct is_specialization_of<Template, Template<Args...>> : std::true_type {};

// Helper variable template
template <template <typename...> class Template, typename T>
inline constexpr bool is_specialization_of_v = is_specialization_of<Template, T>::value;

// Base template
template <typename>
struct is_span : std::false_type {};

// Specialized template
template <typename T, std::size_t N>
struct is_span<std::span<T, N>> : std::true_type {};

// Helper variable template
template <typename T>
inline constexpr bool is_span_v = is_span<T>::value;

} // namespace mpl
