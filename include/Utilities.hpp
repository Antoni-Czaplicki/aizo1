#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <string>
#include <limits>
#include <random>
#include <algorithm>
#include <cstring>

template<typename T>
void generate_data(T *arr, int size) {
    static std::mt19937 gen(std::random_device{}());

    if constexpr (std::is_integral_v<T> && !std::is_same_v<T, bool>) {
        std::uniform_int_distribution<T> dist(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
        for (int i = 0; i < size; ++i) arr[i] = dist(gen);
    } else if constexpr (std::is_floating_point_v<T>) {
        std::uniform_real_distribution<T> dist(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
        for (int i = 0; i < size; ++i) arr[i] = dist(gen);
    }
}

template<>
inline void generate_data<std::string>(std::string *arr, int size) {
    static const char charset[] = "abcdefghijklmnopqrstuvwxyz";
    static std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> len_dist(5, 20);
    std::uniform_int_distribution<int> char_dist(0, strlen(charset) - 1);

    for (int i = 0; i < size; ++i) {
        arr[i].resize(len_dist(gen));
        for (char &c: arr[i]) {
            c = charset[char_dist(gen)];
        }
    }
}

template<>
inline void generate_data<BoardGame>(BoardGame *arr, int size) {
    static std::mt19937 gen(std::random_device{}());

    std::uniform_int_distribution<int> min_players_dist(1, 6);
    std::uniform_int_distribution<int> max_offset_dist(0, 5);
    std::uniform_int_distribution<int> playtime_dist(10, 180);
    std::uniform_int_distribution<int> complexity_dist(1, 5);
    std::uniform_int_distribution<int> joy_dist(1, 5);

    for (int i = 0; i < size; ++i) {
        BoardGame bg;

        // Generate title using existing string generator
        std::string title;
        generate_data(&title, 1);
        bg.title = title;

        // Generate publisher similarly
        std::string publisher;
        generate_data(&publisher, 1);
        bg.publisher = publisher;

        // Generate min and max players
        bg.min_players = min_players_dist(gen);
        int max_offset = max_offset_dist(gen);
        bg.max_players = bg.min_players + max_offset;

        // Ensure max_players is at least min_players (though offset ensures this)
        if (bg.max_players < bg.min_players) {
            bg.max_players = bg.min_players;
        }

        // Generate playtime, complexity, and joy
        bg.playtime = playtime_dist(gen);
        bg.complexity = complexity_dist(gen);
        bg.joy = joy_dist(gen);

        arr[i] = bg;
    }
}

template<typename T>
bool is_sorted(const T *arr, int n) {
    for (int i = 0; i < n - 1; ++i) {
        if (arr[i] > arr[i + 1]) return false;
    }
    return true;
}

#endif
