#ifndef BOARDGAME_HPP
#define BOARDGAME_HPP

#include <string>
#include <iostream>

struct BoardGame {
    std::string title;
    std::string publisher;
    int min_players;
    int max_players;
    int playtime;
    int complexity;
    int joy;

    float fun_factor() const;

    // Add comparison operators
    bool operator>(const BoardGame &other) const {
        return this->fun_factor() > other.fun_factor();
    }

    bool operator<=(const BoardGame &other) const {
        return this->fun_factor() <= other.fun_factor();
    }
};

std::istream &operator>>(std::istream &is, BoardGame &bg);

std::ostream &operator<<(std::ostream &os, const BoardGame &bg);

#endif
