#include <iostream>
#include <map>
#include <array>
#include <chrono>
#include <algorithm>
#include <tuple>


std::chrono::_V2::system_clock::time_point start;
void start_time() { start = std::chrono::high_resolution_clock::now(); }
void stop_time() {
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << duration.count() << " us\n";
}

int main() {
    // Пример 1.
    // Либо что-то сделать, если ключ найден, либо добавить его, если его там не было.
    // Когда мы проверяем есть ли ключ, то уже считаем его положение в мапе, поэтому
    // можно использовать его как подсказку для вставки.
    // 
    // Результаты запуска (-O3):
    // simple insert: 663391 us
    //   hint insert: 576096 us
    // Результаты запуска (без -O3):
    // simple insert: 1291509 us
    //   hint insert: 996172 us

    constexpr size_t sz = 1000000;
    constexpr int lim = 1000000;
    std::array<int, sz> random_keys;
    for (auto& random_key : random_keys) {
        random_key = rand() % lim;
    }
    std::map<int, int> m1, m2;

    std::cout << "First experiment\nsimple insert: ";
    start_time();
    for (auto& random_key : random_keys) {
        if (m1.count(random_key)) {
            // ...
        } else {
            m1.insert({random_key, random_key});
        }
    }
    stop_time();

    std::cout << "  hint insert: ";
    start_time();
    for (auto& random_key : random_keys) {
        auto it = m2.lower_bound(random_key);
        if (it != m2.end() && it->first == random_key) {
            // ...
        } else {
            m2.insert(it, {random_key, random_key});
        }
    }
    stop_time();


    // Пример 2.
    // Известно, что элементы будут добавляться в порядке увеличения ключей (после этого
    // полученный map будет как-то использоваться).
    // 
    // Результаты запуска (-O3):
    // simple insert: 180847 us
    //   hint insert: 74499 us
    // Результаты запуска (без -O3):
    // simple insert: 830135 us
    //   hint insert: 243582 us


    m1.clear();
    m2.clear();
    for (auto& random_key : random_keys) {
        random_key = rand() % (lim * 1000);
    }
    std::sort(random_keys.begin(), random_keys.end());
    
    std::cout << "Second experiment\nsimple insert: ";
    start_time();
    for (auto& random_key : random_keys) {
        m1.insert({random_key, random_key});
    }
    // ...
    stop_time();
    
    
    std::cout << "  hint insert: ";
    start_time();
    auto last_added = m2.begin();
    for (auto& random_key : random_keys) {
        last_added = m2.insert(last_added, {random_key, random_key});
    }
    // ...
    stop_time();


    // Пример 3.
    // Добавление нескольких элементов, стоящих рядом с данным элементом.
    // Например, на сетке при добавлении значения какой-то клетке ее соседям
    // присваивается значение на единицу меньше.
    // (по-хорошему, наверное, стоит написать свой хеш для пар, чтобы близкие
    // по компонентам пары были рядом и в map, но работает и без него)
    //
    // Результаты запуска (-O3):
    // simple insert: 406661 us
    //   hint insert: 313804 us
    // Результаты запуска (без -O3):
    // simple insert: 1312360 us
    //   hint insert: 830551 us


    std::map<std::pair<int, int>, int> m1_d;
    std::map<std::pair<int, int>, int> m2_d;
    std::array<std::pair<int, int>, sz / 4> random_keys_d;
    for (auto& random_key : random_keys_d) {
        random_key = {rand() % lim, rand() % lim};
    }
    std::array<std::pair<int, int>, 4> dirs = {{{1, 0}, {0, 1}, {-1, 0}, {0, -1}}};

    std::cout << "Third experiment\nsimple insert: ";
    
    start_time();
    for (auto& random_key : random_keys_d) {
        m1_d.insert({random_key, random_key.first});
        for (auto& dir : dirs) {
            m1_d.insert({{random_key.first + dir.first, random_key.second + dir.second}, random_key.first - 1});
        }
    }
    stop_time();
    
    
    std::cout << "  hint insert: ";
    start_time();
    for (auto& random_key : random_keys_d) {
        auto [it, inserted] = m2_d.insert({random_key, random_key.first});
        for (auto& dir : dirs) {
            m2_d.insert(it, {{random_key.first + dir.first, random_key.second + dir.second}, random_key.first - 1});
        }
    }
    stop_time();




    return 0;
}