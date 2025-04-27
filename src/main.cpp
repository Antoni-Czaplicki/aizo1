#include <iostream>
#include <string>
#include "Timer.hpp"
#include "BoardGame.hpp"
#include "SortingAlgorithms.hpp"
#include "Utilities.hpp"
#include "FileIO.hpp"

using namespace std;

// Funkcja pomocnicza do parsowania argumentów
struct ProgramParams {
    string mode;
    string algorithm;
    string type;
    string inputFile;
    string outputFile;
    int drunkenness{50};
    int size{};
};

ProgramParams parseArguments(int argc, char *argv[]) {
    ProgramParams params;
    if (argc <= 1) {
        params.mode = "help";
        return params;
    }

    string arg1 = argv[1];
    if (arg1 == "--help") {
        params.mode = "help";
        return params;
    }
    if (arg1 == "--file") {
        if (argc < 5 || argc > 7) {
            cerr << "Nieprawidłowa liczba argumentów dla trybu --file." << endl;
            params.mode = "help";
            return params;
        }
        params.mode = "file";
        params.algorithm = argv[2];
        params.type = argv[3];
        params.inputFile = argv[4];
        if (argc == 6) {
            try {
                params.drunkenness = stoi(argv[5]);
            } catch (const invalid_argument &e) {
                params.outputFile = argv[5];
            }
        }
        if (argc == 7) {
            params.outputFile = argv[5];
            try {
                params.drunkenness = stoi(argv[6]);
            } catch (const invalid_argument &e) {
                cerr << "Drunkenness must be an integer." << endl;
                params.mode = "help";
                return params;
            }
        }
    } else if (arg1 == "--test") {
        if (argc < 5 || argc > 6) {
            cerr << "Nieprawidłowa liczba argumentów dla trybu --test." << endl;
            params.mode = "help";
            return params;
        }
        params.mode = "test";
        params.algorithm = argv[2];
        params.type = argv[3];
        try {
            params.size = stoi(argv[4]);
        } catch (const invalid_argument &e) {
            cerr << "Rozmiar musi być liczbą całkowitą." << endl;
            params.mode = "help";
            return params;
        }
        if (argc == 6) {
            try {
                params.drunkenness = stoi(argv[5]);
            } catch (const invalid_argument &e) {
                cerr << "Drunkenness must be an integer." << endl;
                params.mode = "help";
                return params;
            }
        }
    } else {
        cerr << "Nieznany tryb działania." << endl;
        params.mode = "help";
        return params;
    }

    return params;
}

// Funkcja do wyświetlania pomocy
void showHelp() {
    cout << "FILE TEST MODE:" << endl;
    cout << "    Usage:" << endl;
    cout << "        ./aizo1 --file <algorithm> <type> <inputFile> [outputFile] [drunkenness/pivot type]" << endl;
    cout << "    <algorithm> Sorting algorithm to use: 0 - Insertion, 1 - Heap, 2 - Shell, 3 - Quick, 4 - Drunk Heap."
            << endl;
    cout << "    <type> Data type to load: 0 - int, 1 - float, 2 - string, 3 - BoardGame." << endl;
    cout << "    <inputFile> Input file containing the data to be sorted." << endl;
    cout << "    [outputFile] If provided, the sorted values will be saved to this file." << endl;
    cout << "    [drunkenness] Optional parameter for Drunk Heap Sort (default: 50)." << endl;
    cout << "    [pivot type] Optional parameter for Quick Sort: 0 - Left, 1 - Right, 2 - Middle, 3 - Random (default)."
            << endl;
    cout << "BENCHMARK MODE:" << endl;
    cout << "    Usage:" << endl;
    cout << "        ./aizo1 --test <algorithm> <type> <size> [drunkenness/pivot type]" << endl;
    cout << "    <algorithm> Sorting algorithm to use: 0 - Insertion, 1 - Heap, 2 - Shell, 3 - Quick, 4 - Drunk Heap."
            << endl;
    cout << "    <type> Data type to generate: 0 - int, 1 - float, 2 - string, 3 - BoardGame." << endl;
    cout << "    <size> Number of elements to generate (instance size)." << endl;
    cout << "    [drunkenness] Optional parameter for Drunk Heap Sort (default: 50)." << endl;
    cout << "    [pivot type] Optional parameter for Quick Sort: 0 - Left, 1 - Right, 2 - Middle, 3 - Random (default)."
            << endl;
    cout << "HELP MODE:" << endl;
    cout << "    Usage:" << endl;
    cout << "        ./aizo1 --help" << endl;
    cout << "    Displays this help message." << endl;
    cout << "    Notes:" << endl;
    cout << "    - The help message will also appear if no arguments are provided." << endl;
    cout << "    - Ensure that either --file or --test mode is specified; they are mutually exclusive." << endl;
    cout << "EXAMPLE CALLS:" << endl;
    cout <<
            "Sorting integers using Heap Sort from a file and saving results: ./aizo1 --file 1 0 input.txt sorted_output.txt"
            << endl;
    cout <<
            "Running a benchmark with 1000 randomly generated floats using Insertion Sort: ./aizo1 --test 0 1 1000 results.txt"
            << endl;
}

int main(int argc, char *argv[]) {
    ProgramParams params = parseArguments(argc, argv);

    if (params.mode == "help") {
        showHelp();
        return 0;
    }

    // Mapowanie algorytmów
    enum AlgorithmType {
        INSERTION,
        HEAP,
        SHELL,
        QUICK,
        DRUNK_QUICK
    };

    // Mapowanie typów danych
    enum DataType {
        INT,
        FLOAT,
        STRING,
        BOARDGAME
    };

    AlgorithmType algorithmType;
    DataType dataType;
    PivotType pivotType = PivotType::RANDOM;

    try {
        algorithmType = static_cast<AlgorithmType>(stoi(params.algorithm));
        dataType = static_cast<DataType>(stoi(params.type));
        if (algorithmType == QUICK) {
            if (params.drunkenness > 0 && params.drunkenness < 4) {
                // // Wiem, że nazywa się to "drunkenness", ale było to potrzebne tylko do jednego badania, aby dodać typ pivota, więc pozostaje "drunkenness" bo nie chce mi się wszystkiego przerabiać
                pivotType = static_cast<PivotType>(params.drunkenness);
            }
        }
    } catch (const invalid_argument &e) {
        cerr << "Nieprawidłowy format algorytmu lub typu danych." << endl;
        showHelp();
        return 1;
    }

    Timer timer;
    if (params.mode == "file") {
        // Tryb FILE
        if (dataType == INT) {
            int size;
            auto *arr = load_data<int>(params.inputFile, size);
            timer.start();

            // Wybór algorytmu sortowania
            switch (algorithmType) {
                case INSERTION:
                    insertion_sort(arr, size);
                    break;
                case HEAP:
                    heap_sort(arr, size);
                    break;
                case SHELL:
                    shell_sort(arr, size);
                    break;
                case QUICK:
                    quick_sort(arr, 0, size - 1, pivotType);
                    break;
                case DRUNK_QUICK:
                    drunk_heap_sort(arr, size, params.drunkenness);
                    break;
            }
            timer.stop();

            cout << timer.result() << endl;

            // Zapis do pliku wyjściowego (jeśli podano)
            if (!params.outputFile.empty()) {
                save_data(params.outputFile, arr, size);
            }
        } else if (dataType == FLOAT) {
            int size;
            auto *arr = load_data<float>(params.inputFile, size);
            timer.start();

            // Wybór algorytmu sortowania
            switch (algorithmType) {
                case INSERTION:
                    insertion_sort(arr, size);
                    break;
                case HEAP:
                    heap_sort(arr, size);
                    break;
                case SHELL:
                    shell_sort(arr, size);
                    break;
                case QUICK:
                    quick_sort(arr, 0, size - 1, pivotType);
                    break;
                case DRUNK_QUICK:
                    drunk_heap_sort(arr, size, params.drunkenness);
                    break;
            }
            timer.stop();

            cout << timer.result() << endl;

            // Zapis do pliku wyjściowego (jeśli podano)
            if (!params.outputFile.empty()) {
                save_data(params.outputFile, arr, size);
            }
        } else if (dataType == STRING) {
            int size;
            auto *arr = load_data<std::string>(params.inputFile, size);
            timer.start();

            // Wybór algorytmu sortowania
            switch (algorithmType) {
                case INSERTION:
                    insertion_sort(arr, size);
                    break;
                case HEAP:
                    heap_sort(arr, size);
                    break;
                case SHELL:
                    shell_sort(arr, size);
                    break;
                case QUICK:
                    quick_sort(arr, 0, size - 1, pivotType);
                    break;
                case DRUNK_QUICK:
                    drunk_heap_sort(arr, size, params.drunkenness);
                    break;
            }
            timer.stop();

            cout << timer.result() << endl;

            // Zapis do pliku wyjściowego (jeśli podano)
            if (!params.outputFile.empty()) {
                save_data(params.outputFile, arr, size);
            }
        } else if (dataType == BOARDGAME) {
            int size;
            auto *arr = load_data<BoardGame>(params.inputFile, size);
            timer.start();

            // Wybór algorytmu sortowania
            switch (algorithmType) {
                case INSERTION:
                    insertion_sort(arr, size);
                    break;
                case HEAP:
                    heap_sort(arr, size);
                    break;
                case SHELL:
                    shell_sort(arr, size);
                    break;
                case QUICK:
                    quick_sort(arr, 0, size - 1, pivotType);
                    break;
                case DRUNK_QUICK:
                    drunk_heap_sort(arr, size, params.drunkenness);
                    break;
            }
            timer.stop();

            cout << timer.result() << endl;

            // Zapis do pliku wyjściowego (jeśli podano)
            if (!params.outputFile.empty()) {
                save_data(params.outputFile, arr, size);
            }
        }
    } else if (params.mode == "test") {
        if (dataType == INT) {
            int *arr = new int[params.size];
            generate_data(arr, params.size);

            timer.start();

            // Wybór algorytmu sortowania
            switch (algorithmType) {
                case INSERTION:
                    insertion_sort(arr, params.size);
                    break;
                case HEAP:
                    heap_sort(arr, params.size);
                    break;
                case SHELL:
                    shell_sort(arr, params.size);
                    break;
                case QUICK:
                    quick_sort(arr, 0, params.size - 1, pivotType);
                    break;
                case DRUNK_QUICK:
                    drunk_heap_sort(arr, params.size, params.drunkenness);
                    break;
            }
            timer.stop();

            cout << timer.result() << endl;
            delete[] arr;
        } else if (dataType == FLOAT) {
            float *arr = new float[params.size];
            generate_data(arr, params.size);
            timer.start();

            // Wybór algorytmu sortowania
            switch (algorithmType) {
                case INSERTION:
                    insertion_sort(arr, params.size);
                    break;
                case HEAP:
                    heap_sort(arr, params.size);
                    break;
                case SHELL:
                    shell_sort(arr, params.size);
                    break;
                case QUICK:
                    quick_sort(arr, 0, params.size - 1, pivotType);
                    break;
                case DRUNK_QUICK:
                    drunk_heap_sort(arr, params.size, params.drunkenness);
                    break;
            }
            timer.stop();

            cout << timer.result() << endl;
            delete[] arr;
        } else if (dataType == STRING) {
            string *arr = new string[params.size];
            generate_data(arr, params.size);
            timer.start();

            // Wybór algorytmu sortowania
            switch (algorithmType) {
                case INSERTION:
                    insertion_sort(arr, params.size);
                    break;
                case HEAP:
                    heap_sort(arr, params.size);
                    break;
                case SHELL:
                    shell_sort(arr, params.size);
                    break;
                case QUICK:
                    quick_sort(arr, 0, params.size - 1, pivotType);
                    break;
                case DRUNK_QUICK:
                    drunk_heap_sort(arr, params.size, params.drunkenness);
                    break;
            }
            timer.stop();

            cout << timer.result() << endl;
            delete[] arr;
        } else if (dataType == BOARDGAME) {
            BoardGame *arr = new BoardGame[params.size];
            generate_data(arr, params.size);
            timer.start();

            // Wybór algorytmu sortowania
            switch (algorithmType) {
                case INSERTION:
                    insertion_sort(arr, params.size);
                    break;
                case HEAP:
                    heap_sort(arr, params.size);
                    break;
                case SHELL:
                    shell_sort(arr, params.size);
                    break;
                case QUICK:
                    quick_sort(arr, 0, params.size - 1, pivotType);
                    break;
                case DRUNK_QUICK:
                    drunk_heap_sort(arr, params.size, params.drunkenness);
                    break;
            }
            timer.stop();

            cout << timer.result() << endl;
            delete[] arr;
        }
    }


    bool isJavaInstalled = false;

#ifdef _WIN32
    // Windows
    int result = system("where java >nul 2>nul");
    isJavaInstalled = result == 0;
#else
    // macOS/Linux
    int result = system("which java >/dev/null 2>&1");
    isJavaInstalled = result == 0;
#endif

    if (isJavaInstalled) {
        cout << "Bleee Java. (Chyba że do Minecrafta to spoko jak nie to weź wyjdź)" << endl;
    }


    return 0;
}
