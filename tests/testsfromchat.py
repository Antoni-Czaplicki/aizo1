# ANSI color codes that work on macOS
import os
import random
import subprocess

GREEN = '\033[32m'
RED = '\033[31m'
YELLOW = '\033[33m'
BLUE = '\033[34m'
RESET = '\033[0m'

passed_count = 0
failed_count = 0


def color_print(text, color):
    print(f"{color}{text}{RESET}")


def create_test_files():
    color_print("Creating test input files...", BLUE)

    # Integer test file
    int_data = [random.randint(1, 100000) for _ in range(47)]
    with open("input_int.txt", "w") as f:
        f.write(f"{len(int_data)}\n")  # Write size first
        for num in int_data:
            f.write(f"{num}\n")

    # Float test file
    float_data = [round(random.uniform(1, 100), 2) for _ in range(47)]
    with open("input_float.txt", "w") as f:
        f.write(f"{len(float_data)}\n")
        for num in float_data:
            f.write(f"{num}\n")

    # String test file
    string_data = ["apple", "banana", "cherry", "date", "elderberry", "fig", "grape"] * 7
    string_data = string_data[:47]
    with open("input_string.txt", "w") as f:
        f.write(f"{len(string_data)}\n")
        for s in string_data:
            f.write(f"{s}\n")

    # BoardGame test file
    games = ["Chess", "Monopoly", "Checkers", "Catan", "Risk", "Scrabble", "Poker"] * 7
    games = games[:47]
    with open("input_boardgame.txt", "w") as f:
        f.write(f"{len(games)}\n")
        for game in games:
            publisher = random.choice(["PublisherA", "PublisherB", "PublisherC"])
            min_p = random.randint(2, 4)
            max_p = random.randint(min_p, 8)
            time = random.randint(15, 180)
            complexity = random.randint(1, 5)
            joy = random.randint(1, 10)
            f.write(f"{game},{publisher},{min_p},{max_p},{time},{complexity},{joy}\n")
def run_test_case(cmd, description, check_output=False):
    global passed_count, failed_count

    color_print(f"Running test: {description}", BLUE)
    result = subprocess.run(cmd, capture_output=True, text=True)

    success = False
    if result.returncode == 0:
        if check_output:
            output = result.stdout.strip()
            if output.replace('.', '', 1).isdigit():
                success = True
            else:
                color_print(f"Invalid output format: {output}", YELLOW)
        else:
            success = True
    else:
        color_print(f"Error output:\n{result.stderr.strip()}", RED)

    if success:
        color_print(f"PASSED: {description}", GREEN)
        passed_count += 1
    else:
        color_print(f"FAILED: {description}", RED)
        failed_count += 1

    return success


def test_valid_scenarios():
    algorithms = {0: "Insertion", 1: "Heap", 2: "Shell", 3: "Quick", 4: "Drunk_Ins"}
    data_types = {0: "int", 1: "float", 2: "string", 3: "BoardGame"}
    test_size = 100  # Elements for benchmark tests

    # Test --file mode
    for algo_num, algo_name in algorithms.items():
        for data_num, data_type in data_types.items():
            input_file = f"input_{data_type}.txt"
            output_file = f"output_{algo_name}_{data_type}.txt"
            cmd = [
                "./aizo1", "--file",
                str(algo_num),
                str(data_num),
                input_file,
                output_file
            ]
            if run_test_case(cmd, f"FILE {algo_name} sort with {data_type}"):
                if not os.path.exists(output_file):
                    print(f"{YELLOW}Output file {output_file} not created!{RESET}")

    # Test --test mode (no output file)
    for algo_num, algo_name in algorithms.items():
        for data_num, data_type in data_types.items():
            cmd = [
                "./aizo1", "--test",
                str(algo_num),
                str(data_num),
                str(test_size)
            ]
            run_test_case(cmd,
                          f"TEST {algo_name} sort with {data_type}", check_output=True)


def test_invalid_scenarios():
    # Test help mode
    run_test_case(["./aizo1", "--help"], "Help mode")
    run_test_case(["./aizo1"], "No arguments mode")

    # Invalid mode
    run_test_case(["./aizo1", "--invalid"], "Invalid mode")

    # Invalid algorithm
    run_test_case(
        ["./aizo1", "--file", "5", "0", "input_int.txt", "output.txt"],
        "Invalid algorithm"
    )

    # Invalid data type
    run_test_case(
        ["./aizo1", "--file", "0", "4", "input_int.txt", "output.txt"],
        "Invalid data type"
    )

    # Insufficient arguments for --file
    run_test_case(
        ["./aizo1", "--file", "0", "0"],
        "Insufficient arguments for --file"
    )

    # Insufficient arguments for --test
    run_test_case(
        ["./aizo1", "--test", "0", "0"],
        "Insufficient arguments for --test"
    )


def main():
    global passed_count, failed_count
    check_if_compiled = os.path.exists("./aizo1")
    if not check_if_compiled:
        print("Compiled program not found.")
        return
    create_test_files()

    color_print("\n=== Testing valid scenarios ===", BLUE)
    test_valid_scenarios()

    color_print("\n=== Testing invalid scenarios ===", BLUE)
    test_invalid_scenarios()

    total = passed_count + failed_count
    color_print("\n=== Test Summary ===", BLUE)
    color_print(f"Passed: {passed_count}/{total}", GREEN)
    color_print(f"Failed: {failed_count}/{total}", RED)


if __name__ == "__main__":
    main()
