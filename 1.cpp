#include <array>
#include <iostream>


template <int N>
constexpr int det(const std::array<std::array<int, N>, N>& a) {
    int result = 0;
    std::array<int, N> perm{};
    for (int i = 0; i < N; ++i) {
        (&std::get<0>(perm))[i] = i;
    }
    int sign = 1;
    while (true) {
        int curr_res = 1;
        for (int kk = 0; kk < N; ++kk) {
            curr_res *= a[kk][(&std::get<0>(perm))[kk]];
        }
        if (sign) {
            result += curr_res;
        } else {
            result -= curr_res;
        }
        bool found = false;
        int i = N - 1;
        for (; i > 0 && !found; --i) {
            if ((&std::get<0>(perm))[i] > (&std::get<0>(perm))[i-1]) {
                found = true;
                break;
            }
        }
        if (!found) {
            break;
        }
        int k = N - 1;
        while ((&std::get<0>(perm))[i-1] >= (&std::get<0>(perm))[k]) {
            k--;
        }
        sign = (sign + 1) % 2;
        int tmp = (&std::get<0>(perm))[i-1];
        (&std::get<0>(perm))[i-1] = (&std::get<0>(perm))[k];
        (&std::get<0>(perm))[k] = tmp;
        int l = i, r = N-1;
        while (l < r) {
            sign = (sign + 1) % 2;
            tmp = (&std::get<0>(perm))[l];
            (&std::get<0>(perm))[l] = (&std::get<0>(perm))[r];
            (&std::get<0>(perm))[r] = tmp;
            l++;
            r--;
        }
    }
    return result;
}

int main() {
    constexpr std::array<std::array<int, 3>, 3> A = {{
        {0, 1, 2}
        ,{1, 2, 3}
        ,{2, 3, 7}
    }};
    constexpr int res = det<3>(A);
    std::cout << res << std::endl;
}