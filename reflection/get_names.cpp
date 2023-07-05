#include <array>
#include <string_view>
#include <utility>
#include <algorithm>

#include <stdio.h>

template<typename... Args>
struct pack {};

template<typename Result, typename Input>
struct reverse_impl;

template<typename... Result, typename First, typename... Rest>
struct reverse_impl<pack<Result...>, pack<First, Rest...>> {
    using type = typename reverse_impl<pack<First, Result...>, pack<Rest...>>::type;
};

template<typename... Result>
struct reverse_impl<pack<Result...>, pack<>> {
    using type = pack<Result...>;
};

template<typename Pack>
struct reverse;

template<typename... Args>
struct reverse<pack<Args...>> {
    using type = typename reverse_impl<pack<>, pack<Args...>>::type;
};

template <char C>
class CTChar {
public:
    constexpr static const char value = C;
};


template <typename ...Chars>
class CTString {
public:
    constexpr static const char value[] = {Chars::value ..., '\0'};

    consteval static size_t size() noexcept { return sizeof(value); }

};

// Not standard compliant, only a part of the GNU extension
// template <typename T, T... chars>
// constexpr auto operator""_cts() {
//     return CTString<CTChar<chars>...>{};
// }

template <typename ...Chars>
struct CTStringSliceResult;


template <typename ...Chars>
struct CTStringSliceResult<pack<Chars...>> {
    using type = CTString<Chars...>;
};


template <unsigned End, typename Char, typename ...Chars>
struct CTStringSliceUpImpl;

template <unsigned End, typename ...Chars>
struct CTStringSliceUp {
    using type = CTStringSliceUpImpl<End, Chars...>::type;
};

template <typename ...Chars>
struct CTStringSliceUp<0, Chars...> {
    using reversed_type = typename reverse<pack<Chars...>>::type;

    using type = CTStringSliceResult<reversed_type>::type;
};

template <unsigned End, typename Char, typename ...Chars>
struct CTStringSliceUpImpl {
    using type = CTStringSliceUp<End - 1, Chars...>::type;
};

template <unsigned End, typename ...Chars>
struct CTStringSliceUpUnpack;

template <unsigned End, typename ...Chars>
struct CTStringSliceUpUnpack<End, pack<Chars...>> {
    using type = CTStringSliceUp<End, Chars...>::type;
};

template <unsigned End, typename ...Chars>
struct CTStringSliceReverse {
    using reversed_type = typename reverse<pack<Chars...>>::type;

    using type = CTStringSliceUpUnpack<sizeof...(Chars) - End, reversed_type>::type;
};

template <unsigned Begin, unsigned End, typename Char, typename ...Chars>
struct CTStringSliceDown {
    using type = CTStringSliceDown<Begin - 1, End - 1, Chars...>::type;
};

template <unsigned End, typename Char, typename ...Chars>
struct CTStringSliceDown<0, End, Char, Chars...> {
    using type = CTStringSliceReverse<End, Char, Chars...>::type;
};

template <unsigned Begin, unsigned End, typename ...Chars>
struct CTSlice;

template <unsigned Begin, unsigned End, typename ...Chars>
struct CTSlice<Begin, End, CTString<Chars...>> {
    
    using type = CTStringSliceDown<Begin, End, Chars...>::type;
    static_assert(Begin <= End);
    static_assert(End <= sizeof...(Chars));

};

template<size_t N>
struct StringLiteral {
    constexpr StringLiteral(const char (&str)[N]) {
        std::copy_n(str, N, value);
    }
    
    char value[N];
};

template<StringLiteral literal, size_t... Idx>
constexpr auto MakeCTStringHelper(std::index_sequence<Idx...>) {
    return CTString<CTChar<literal.value[Idx]>...>{};
}

template<StringLiteral literal>
constexpr auto MakeCTString() {
    return MakeCTStringHelper<literal>(std::make_index_sequence<sizeof(literal.value) - 1>{});
}

// String is an instace of CTString

template <typename String, unsigned i = 0, unsigned c = 0> static constexpr unsigned 
countargs ( ) noexcept {
    if constexpr (i > String::size() - 1) {
        return (i == 0 ? 0 : c + 1);
    } else {
        return countargs<String, i + 1, c + (String::value[i] == ',')>();
    }
}

template<typename String>
class CTSeparator {
    public:

    template <unsigned N = countargs<String>()>
    static constexpr std::array<const char *, N>
    get_segments() noexcept {
        return  [] <std::size_t ...Is> (std::index_sequence<Is...>) constexpr {
            return std::array<const char *, N> {
                nth_segment<Is>::value...
            };
        }(std::make_index_sequence<N>{});
    }

    // private:
    
   

    template <unsigned n, unsigned count = 0, unsigned index = 0> static constexpr unsigned
    start_after_nth_comma () noexcept {
        if constexpr (n == 0 || index >= String::size() - 1) {
            return (n != 0) * index;
        } else if constexpr (String::value[index] == ',' && count + 1 == n) {
            return (index + 1 + 1);
        } else {
            return start_after_nth_comma<n, count + (String::value[index] == ','), index + 1>();
        }
    }

    template <unsigned n, unsigned count = 0, unsigned index = 0> static constexpr unsigned 
    end_after_nth_comma() noexcept {
        if constexpr ((index >= String::size() - 1) || (String::value[index] == ',' && count + 1 == n + 1)) {
            return index;
        } else {
            return end_after_nth_comma<n, count + (String::value[index] == ','), index + 1>();
        }
    }

    template <unsigned n>
    using nth_segment = CTSlice<start_after_nth_comma<n>(), end_after_nth_comma<n>(), String>::type;
};

#define get_names(...) CTSeparator< decltype(MakeCTString<StringLiteral(#__VA_ARGS__)>()) >::get_segments();




