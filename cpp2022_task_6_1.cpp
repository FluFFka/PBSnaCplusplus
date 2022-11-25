#include <iostream>
#include <type_traits>

template <char ...c> struct TString { };

template <typename T, T ...c>
constexpr TString<c...> operator"" _s() {
    return TString<c...>();
}

template <char ...lc, char ...rc>
constexpr TString<lc..., rc...> operator+(const TString<lc...> &lhs, const TString<rc...> &rhs) {
    return TString<lc..., rc...>();
}


template <char ...lc, char ...rc>
constexpr bool operator==(const TString<lc...> &lhs, const TString<rc...> &rhs) {
    return std::is_same_v<TString<lc...>, TString<rc...>>;
}


int main()
{
    constexpr auto hello = "hello"_s + " world"_s;
    static_assert(hello == "hello world"_s);
    return 0;
}