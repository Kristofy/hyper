#pragma once
#include <cstddef>
#include <string_view>
#include <type_traits>
#include <utility>

/// @brief compile time reflection namespace
namespace reflection {

namespace detail {
struct any {
  template<typename Type>
  operator Type();
};

/// @brief Get the the given type as a string includeing CV qualifliers
/// @tparam T the type
/// @return a string view of the type as a string
template<typename T>
constexpr std::string_view GetTypeNameCV() {
#if defined(__clang__)
  constexpr auto prefix   = std::string_view{"[T = "};
  constexpr auto suffix   = "]";
  constexpr auto function = std::string_view{__PRETTY_FUNCTION__};
#elif defined(__GNUC__)
  constexpr auto prefix   = std::string_view{"with T = "};
  constexpr auto suffix   = "; ";
  constexpr auto function = std::string_view{__PRETTY_FUNCTION__};
#elif defined(_MSC_VER)
  constexpr auto prefix   = std::string_view{"GetTypeName<"};
  constexpr auto suffix   = ">(void)";
  constexpr auto function = std::string_view{__FUNCSIG__};
#else
#error Unsupported compiler
#endif

  const auto start = function.find(prefix) + prefix.size();
  const auto end   = function.find(suffix);
  const auto size  = end - start;

  return function.substr(start, size);
}

} // namespace detail

/// @brief Returns the given type as a string desregarding the CV qualifiers of the type
/// @tparam T
/// @return the given type as a string
template<typename T>
constexpr std::string_view GetTypeName() {
  return detail::GetTypeNameCV<std::remove_cv_t<std::remove_reference_t<T>>>();
}

/// @brief For an aggregate structure return the number of members inside the function
/// @tparam T and aggregate structure
/// @tparam Index Should be set automatically
/// @return the number of members inside structure T
template<typename T, std::size_t Index = 20>
constexpr std::size_t NumberOfMembers() {
  using Type = std::remove_cvref_t<T>;
  if constexpr (requires { requires std::is_empty_v<Type>; }) {
    return 0;
  } else {
    constexpr std::size_t size = []<std::size_t... I>(std::index_sequence<I...>) {
      return [](auto... args) {
        if constexpr (requires { T{args...}; }) {
          return sizeof...(args);
        } else {
          return 0;
        }
      }(((void)I, detail::any{})...);
    }
    (std::make_index_sequence<Index>{});

    if constexpr (size != 0) {
      return size;
    } else if constexpr (Index > 1) {
      return NumberOfMembers<T, Index - 1>();
    } else {
      static_assert(std::is_void_v<T>, "Type has too many members, consider updating the maximum to check");
      return 0;
    }
  }
}

/// @brief Applies a Function for every member of a structure
/// @tparam Struct the type of the structure (will be deduced)
/// @tparam Fn the type fo the function (will be deduced)
/// @param object the object for witch the function will be applied for every member
/// @param visitor the function to apply for every member
/// @return the same as the visitor
template<typename Struct, typename Fn>
constexpr decltype(auto) StructEach(Struct&& object, Fn&& visitor) {
  constexpr std::size_t count = NumberOfMembers<std::remove_cvref_t<Struct>>();

  if constexpr (count == 0) {
    return visitor();
  } else if constexpr (count == 1) {
    auto& [m1] = object;
    return visitor(m1);
  } else if constexpr (count == 2) {
    auto& [m1, m2] = object;
    return visitor(m1, m2);
  } else if constexpr (count == 3) {
    auto& [m1, m2, m3] = object;
    return visitor(m1, m2, m3);
  } else if constexpr (count == 4) {
    auto& [m1, m2, m3, m4] = object;
    return visitor(m1, m2, m3, m4);
  } else if constexpr (count == 5) {
    auto& [m1, m2, m3, m4, m5] = object;
    return visitor(m1, m2, m3, m4, m5);
  } else if constexpr (count == 6) {
    auto& [m1, m2, m3, m4, m5, m6] = object;
    return visitor(m1, m2, m3, m4, m5, m6);
  } else if constexpr (count == 7) {
    auto& [m1, m2, m3, m4, m5, m6, m7] = object;
    return visitor(m1, m2, m3, m4, m5, m6, m7);
  } else if constexpr (count == 8) {
    auto& [m1, m2, m3, m4, m5, m6, m7, m8] = object;
    return visitor(m1, m2, m3, m4, m5, m6, m7, m8);
  } else if constexpr (count == 9) {
    auto& [m1, m2, m3, m4, m5, m6, m7, m8, m9] = object;
    return visitor(m1, m2, m3, m4, m5, m6, m7, m8, m9);
  } else if constexpr (count == 10) {
    auto& [m1, m2, m3, m4, m5, m6, m7, m8, m9, m10] = object;
    return visitor(m1, m2, m3, m4, m5, m6, m7, m8, m9, m10);
  } else if constexpr (count == 11) {
    auto& [m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11] = object;
    return visitor(m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11);
  } else if constexpr (count == 12) {
    auto& [m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12] = object;
    return visitor(m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12);
  } else if constexpr (count == 13) {
    auto& [m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13] = object;
    return visitor(m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13);
  } else if constexpr (count == 14) {
    auto& [m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14] = object;
    return visitor(m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14);
  } else if constexpr (count == 15) {
    auto& [m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14, m15] = object;
    return visitor(m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14, m15);
  } else if constexpr (count == 16) {
    auto& [m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14, m15, m16] = object;
    return visitor(m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14, m15, m16);
  } else if constexpr (count == 17) {
    auto& [m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14, m15, m16, m17] = object;
    return visitor(m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14, m15, m16, m17);
  } else if constexpr (count == 18) {
    auto& [m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14, m15, m16, m17, m18] = object;
    return visitor(m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14, m15, m16, m17, m18);
  } else if constexpr (count == 19) {
    auto& [m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14, m15, m16, m17, m18, m19] = object;
    return visitor(m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14, m15, m16, m17, m18, m19);
  } else if constexpr (count == 20) {
    auto& [m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14, m15, m16, m17, m18, m19, m20] = object;
    return visitor(m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14, m15, m16, m17, m18, m19, m20);
  } else if constexpr (count == 21) {
    auto& [m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14, m15, m16, m17, m18, m19, m20, m21] = object;
    return visitor(m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14, m15, m16, m17, m18, m19, m20, m21);
  } else if constexpr (count == 22) {
    auto& [m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14, m15, m16, m17, m18, m19, m20, m21, m22] = object;
    return visitor(m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14, m15, m16, m17, m18, m19, m20, m21, m22);
  } else if constexpr (count == 23) {
    static_assert(std::is_void_v<decltype(object)>, "Too many members");
    return decltype(visitor()){};
  }
}

} // namespace reflection
