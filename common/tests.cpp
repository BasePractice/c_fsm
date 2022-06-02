#include <cstdlib>
#include <iostream>

template<typename T>
constexpr T const_abs(T x) {
    return x > 0 ? x : -x;
}

static void test_size() {
    static_assert(sizeof(char) == 1);
    static_assert(sizeof(short) == 2);
    static_assert(sizeof(short int) == 2);
    static_assert(sizeof(int) == 4);
    static_assert(sizeof(long) == 4);
    static_assert(sizeof(long int) == 4);
    static_assert(sizeof(long long) == 8);
    static_assert(sizeof(double) == 8);
    static_assert(sizeof(long double) == 16);
    static_assert(sizeof(float) == 4);
}

static void test_compare() {
    constexpr double x = 2.5, y = 2.5;
    static_assert(const_abs(x - y) < 0.0000000000000000000000000000000000000002f);
    static_assert(
            static_cast<float>(std::numeric_limits<int>::max()) ==
            static_cast<float>(static_cast<long long>(std::numeric_limits<int>::max()) + 1)
    );
}

int main() {
    test_size();
    test_compare();
    return EXIT_SUCCESS;
}