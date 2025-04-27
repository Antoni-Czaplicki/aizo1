#ifndef FILEIO_HPP
#define FILEIO_HPP

#include <string>
#include <fstream>
#include <stdexcept>

template<typename T>
T *load_data(const std::string &filename, int &size) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }

    file >> size;
    file.ignore();

    T *data = new T[size];

    for (int i = 0; i < size; ++i) {
        file >> data[i];
    }

    return data;
}

template<typename T>
void save_data(const std::string &filename, const T *arr, int size) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }

    file << size << '\n';
    for (int i = 0; i < size; ++i) {
        file << arr[i] << '\n';
    }
}

#endif
