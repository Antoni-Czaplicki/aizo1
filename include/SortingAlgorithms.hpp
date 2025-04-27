#ifndef SORTINGALGORITHMS_HPP
#define SORTINGALGORITHMS_HPP

#include <algorithm>
#include <random>
#include "Utilities.hpp"

// Insertion Sort
template<typename T>
void insertion_sort(T *arr, int n) {
    for (int i = 1; i < n; ++i) {
        T key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

// Heap Sort
template<typename T>
void heapify(T *arr, int n, int i) {
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < n && arr[left] > arr[largest])
        largest = left;

    if (right < n && arr[right] > arr[largest])
        largest = right;

    if (largest != i) {
        std::swap(arr[i], arr[largest]);
        heapify(arr, n, largest);
    }
}

template<typename T>
void heap_sort(T *arr, int n) {
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i);

    for (int i = n - 1; i > 0; i--) {
        std::swap(arr[0], arr[i]);
        heapify(arr, i, 0);
    }
}

// Shell Sort
enum class GapSequence { SHELL, CIURA };

template<typename T>
void shell_sort(T *arr, int n, GapSequence seq = GapSequence::CIURA) {
    int *gaps = nullptr;
    int gaps_count = 0;

    if (seq == GapSequence::SHELL) {
        int temp = n / 2;
        gaps_count = 0;
        while (temp > 0) {
            gaps_count++;
            temp /= 2;
        }
        gaps = new int[gaps_count];
        int index = 0;
        for (int gap = n / 2; gap > 0; gap /= 2) {
            gaps[index++] = gap;
        }
    } else {
        static const int ciura_gaps[] = {701, 301, 132, 57, 23, 10, 4, 1};

        gaps_count = 0;
        for (int ciura_gap: ciura_gaps) {
            if (ciura_gap <= n) {
                gaps_count++;
            }
        }

        gaps = new int[gaps_count];
        int index = 0;
        for (int ciura_gap: ciura_gaps) {
            if (ciura_gap <= n) {
                gaps[index++] = ciura_gap;
            }
        }
    }

    for (int g = 0; g < gaps_count; ++g) {
        int gap = gaps[g];
        for (int i = gap; i < n; ++i) {
            T temp = arr[i];
            int j;
            for (j = i; j >= gap && arr[j - gap] > temp; j -= gap) {
                arr[j] = arr[j - gap];
            }
            arr[j] = temp;
        }
    }

    delete[] gaps;
}

// Quick Sort
enum class PivotType { LEFT, RIGHT, MIDDLE, RANDOM };

template<typename T>
int partition(T *arr, int low, int high, PivotType pt) {
    int pivot_idx = low;

    switch (pt) {
        case PivotType::RIGHT: pivot_idx = high;
            break;
        case PivotType::MIDDLE: pivot_idx = low + (high - low) / 2;
            break;
        case PivotType::RANDOM: pivot_idx = low + rand() % (high - low + 1);
            break;
        default: break;
    }

    std::swap(arr[pivot_idx], arr[high]);
    T pivot = arr[high];

    int i = low - 1;
    for (int j = low; j < high; ++j) {
        if (arr[j] <= pivot) {
            i++;
            std::swap(arr[i], arr[j]);
        }
    }
    std::swap(arr[i + 1], arr[high]);
    return i + 1;
}

template<typename T>
void quick_sort(T *arr, int low, int high, PivotType pt) {
    if (low < high) {
        int pi = partition(arr, low, high, pt);
        quick_sort(arr, low, pi - 1, pt);
        quick_sort(arr, pi + 1, high, pt);
    }
}


// Drunk Heap Sort
template<typename T>
void drunk_heapify(T *arr, int n, int i, int drunkenness) {
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < n && arr[left] > arr[largest])
        largest = left;
    if (right < n && arr[right] > arr[largest])
        largest = right;

    if (drunkenness > 0 && (rand() % 100 < drunkenness)) {
        std::vector<int> options;
        if (left < n) options.push_back(left);
        if (right < n) options.push_back(right);
        if (!options.empty()) {
            int random_pick = options[rand() % options.size()];
            largest = random_pick;
        }
    }

    if (largest != i) {
        std::swap(arr[i], arr[largest]);
        drunk_heapify(arr, n, largest, drunkenness);
    }
}

template<typename T>
void drunk_heap_sort(T *arr, int n, int drunkenness) {
    for (int i = n / 2 - 1; i >= 0; i--) {
        drunk_heapify(arr, n, i, drunkenness);
    }

    for (int i = n - 1; i > 0; i--) {
        std::swap(arr[0], arr[i]);
        drunk_heapify(arr, i, 0, drunkenness);
    }

    // If not sorted, try again with reduced drunkenness
    if (!is_sorted(arr, n)) {
        drunk_heap_sort(arr, n, drunkenness / 2);
    }
}

#endif
