#include "BoardGame.hpp"
#include <algorithm>

float BoardGame::fun_factor() const {
    float base = (max_players + min_players) * complexity * joy / (playtime + 1.0f);
    float a_count = std::count(publisher.begin(), publisher.end(), 'a');
    return base + title.length() + a_count;
}

std::istream &operator>>(std::istream &is, BoardGame &bg) {
    std::string line;
    getline(is, line);
    size_t pos = 0, prev = 0;

    const auto next_part = [&](char delim = ',') {
        pos = line.find(delim, prev);
        std::string part = line.substr(prev, pos - prev);
        prev = pos + 1;
        return part;
    };

    bg.title = next_part();
    bg.publisher = next_part();
    bg.min_players = stoi(next_part());
    bg.max_players = stoi(next_part());
    bg.playtime = stoi(next_part());
    bg.complexity = stoi(next_part());
    bg.joy = stoi(next_part());
    return is;
}

std::ostream &operator<<(std::ostream &os, const BoardGame &bg) {
    os << bg.title << "," << bg.publisher << ","
            << bg.min_players << "," << bg.max_players << ","
            << bg.playtime << "," << bg.complexity << "," << bg.joy;
    return os;
}
