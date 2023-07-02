#include <array>
#include <string_view>
#include <utility>

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

template <typename T, T... chars>
constexpr auto operator""_cts() {
    return CTString<CTChar<chars>...>{};
}

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

#define convert(str) str##_cts

#define get_names(...) []() constexpr { \
    return CTSeparator<decltype(convert(#__VA_ARGS__))>::get_segments(); \
}() 

// #define TIE(...) []{ \
// constexpr char arg_str[] = #__VA_ARGS__; \
// constexpr const unsigned n = countargs(arg_str); \
// }()

void test() {
    // constexpr auto arr = CTSeparator(std::string_view("he, hi, ho, asdfd, fsadfsdf, d"));
    // std::cout << String::size() << std::endl;
    // constexpr auto arr = CTSeparator<decltype("ha, hi, hu, he, hajaj"_cts)>::get_segments<5>();
    
    // std::cout << '|';
    // for(auto s : arr) std::cout << s << '|';
    // std::cout << '\n';

    // auto brr = get_names(he, hi, ho, asdfd, fsaddf, d);



    // constexpr auto k0 = nth_segment<5>(std::string_view("he, hi, ho, asdfd, fsadfsdf, d"));
    // constexpr auto k1 = end_after_nth_comma<5>(std::string_view("he, hi, ho, asdfd, fsadfsdf, d"));
    // std::cout << '"' << k0 << '"' << std::endl;
    // std::cout << '"' << k1 << '"' << std::endl;
}


int main() {    
    // test();

    // using String = decltype("Hello huhu"_cts);
    
    // auto xs = String::value;
    
    // std::cout << xs << std::endl;

    // using CutString = typename CTSlice<0, 5, String>::type;
    // auto cut = CutString::value;

    // std::cout << '"' << cut << '"' << std::endl;

    // size_t k = String::size();
    constexpr auto arr0 = get_names(ha, hi, hu, he, hajaj);
    constexpr auto arr1 = get_names(ha, hi, hu, he, hajaj);
    constexpr auto arr2 = get_names(hi, hu, he);

    for(int i = 0; i < arr0.size(); i++) {
        puts(arr0[i]);
        puts(arr1[i]);
    }

    for(int i = 0; i < arr2.size(); i++) {
        puts(arr2[i]);
    }

    return 0;   
}



// std::array<std::string_view, countargs(str)> segs = generate_segments(str);




