#include <type_traits>
#include <cstdint>
#include <iostream>

template<class...>
struct are_same;

template<>
struct are_same<> : std::true_type {};

template<class T1>
struct are_same<T1> : std::true_type {};

template<class T1, class T2, class... Targs>
struct are_same<T1, T2, Targs...> : std::integral_constant<bool, std::is_same<std::decay_t<T1>, std::decay_t<T2>>::value && are_same<T2, Targs...>::value> {};

template<class... Targs>
constexpr bool are_same_v = are_same<Targs...>::value;

int main()
{
    static_assert(are_same_v<int &, const int32_t &&, volatile signed int>, "compile assert");

    
    std::cout << are_same_v<int &, const int32_t &&, volatile signed int> << std::endl;
    std::cout << are_same_v<int, int32_t, signed int> << std::endl;
    return 0;
}