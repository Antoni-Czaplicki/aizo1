import random
import string
import subprocess
from datetime import datetime
from pathlib import Path

import matplotlib.pyplot as plt
import pandas as pd
import seaborn as sns

# Configuration
RUN_COUNT = 100  # Number of runs for each test

EXPERIMENTS_TO_RUN = [1, 2, 3]

# Experiment 1: Algorithm performance vs dataset size
E1_ALGORITHMS_TO_TEST = [0, 1, 2, 3, 4]

# Experiment 2: Impact of initial data distribution
E2_SIZE = 50000
E2_DATA_TYPE = 0

# Experiment 3: Data type impact analysis
E3_ALGORITHM_TO_TEST = 3
E3_SIZE = 50000
E3_DATA_TYPES = [0, 1, 2, 3]

ALGORITHMS = {
    0: "Insertion",
    1: "Heap",
    2: "Shell",
    3: "Quick",
    4: "Drunk"
}

DATA_TYPES = {
    0: ("int", lambda s: [random.randint(-2147483648, 2147483647) for _ in range(s)]),
    1: ("float", lambda s: [random.uniform(-1e6, 1e6) for _ in range(s)]),
    2: ("string", lambda s: [
        f"{''.join(random.choices(string.printable, k=random.randint(1, 10)))}"
        for _ in range(s)
    ]),
    3: ("boardgame", lambda s: [
        f"Game_{i},Publisher_{i},{random.randint(1, 4)},{random.randint(2, 8)},"
        f"{random.randint(5, 480)},{random.randint(1, 10)},{random.randint(1, 10)}"
        for i in range(s)
    ])
}

# Directory structure
BASE_DIR = Path(__file__).parent
RESULTS_DIR = BASE_DIR / f"results_E{EXPERIMENTS_TO_RUN}:E1{'|'.join([ALGORITHMS[algo] for algo in E1_ALGORITHMS_TO_TEST])}:E2{DATA_TYPES[E2_DATA_TYPE][0]}:E3{ALGORITHMS[E3_ALGORITHM_TO_TEST]}_{datetime.now().strftime('%Y%m%d_%H%M%S')}"
INPUTS_DIR = BASE_DIR / "inputs"


def setup_directories():
    """Create required directory structure"""
    for d in [RESULTS_DIR, INPUTS_DIR]:
        d.mkdir(exist_ok=True)
    for experiment in EXPERIMENTS_TO_RUN:
        study_dir = RESULTS_DIR / f"badanie{experiment}"
        study_dir.mkdir(exist_ok=True)


def generate_input_file(size, distribution="random", data_type=0):
    """Generate input file with specified characteristics"""
    dtype_name, generator = DATA_TYPES[data_type]
    filename = INPUTS_DIR / f"{dtype_name}_{distribution}_{size}.txt"

    arr = generator(size)

    if distribution == "ascending" and data_type != 3:  # Avoid sorting boardgame data as we don't need it and it is not easily sortable
        arr.sort()
    elif distribution == "descending" and data_type != 3:
        arr.sort(reverse=True)
    elif distribution.startswith("part"):
        sorted_part = int(distribution.split("_")[1]) / 100
        split_idx = int(sorted_part * size)
        arr[:split_idx] = sorted(arr[:split_idx])

    with open(filename, "w") as f:
        f.write(f"{size}\n")
        for item in arr:
            f.write(f"{item}\n")
    return filename


def run_algorithm(mode, algorithm, data_type, size=None, input_file=None, drunkenness=50, run_index=None):
    """Execute the C++ program and return timing results"""
    cmd = [
        "./aizo1",
        f"--{mode}",
        str(algorithm),
        str(data_type)
    ]

    if input_file:
        cmd.append(str(input_file))
    elif size:
        cmd.append(str(size))

    if mode == "file":
        cmd.append(str(RESULTS_DIR / "temp.txt"))

    if algorithm == 4:  # Drunk algorithm
        cmd.append(str(drunkenness))

    try:
        print(
            f"{datetime.now()}{f' - Run index: {run_index}' if run_index is not None else ''} - Running command: {' '.join([str(arg) for arg in cmd])}")
        result = subprocess.run(cmd, capture_output=True, text=True, check=True)
        return int(result.stdout.split("\n")[0].strip())
    except subprocess.CalledProcessError as e:
        print(f"Error running: {' '.join([str(arg) for arg in cmd])}")
        print(f"Error: {e.stderr}")
        return None
    except ValueError:
        print(f"Invalid output format for command: {cmd}")
        return None


def badanie1():
    """Study 1: Algorithm performance vs dataset size"""
    sizes = [10000, 20000, 40000, 80000, 160000]
    results = []

    # Standard algorithms
    for algo in E1_ALGORITHMS_TO_TEST:
        if algo == 4:  # Skip Drunk algorithm for this part
            continue
        for size in sizes:
            times = []
            for run_index in range(RUN_COUNT):
                time = run_algorithm("test", algo, 0, size, run_index=run_index)
                times.append(time)

            results.append({
                "Algorithm": ALGORITHMS[algo],
                "Size": size,
                "Average": sum(times) / len(times),
                "Min": min(times),
                "Max": max(times),
                "Standard Deviation": pd.Series(times).std()
            })

    if 4 in E1_ALGORITHMS_TO_TEST:
        # Drunk algorithm analysis
        drunkenness_levels = [10, 30, 50, 70, 90]

        for drunk_size in sizes:
            for level in drunkenness_levels:
                times = []
                for run_index in range(RUN_COUNT):
                    time = run_algorithm("test", 4, 0, drunk_size, drunkenness=level, run_index=run_index)
                    times.append(time)

                results.append({
                    "Algorithm": f"Drunk_{level}",
                    "Size": drunk_size,
                    "Average": sum(times) / len(times),
                    "Min": min(times),
                    "Max": max(times),
                    "Standard Deviation": pd.Series(times).std()
                })

    # Save and plot
    df = pd.DataFrame(results)
    output_file = RESULTS_DIR / "badanie1" / "results.csv"
    df.to_csv(output_file, index=False)

    plt.figure(figsize=(12, 8))
    sns.lineplot(data=df, x="Size", y="Average", hue="Algorithm")
    plt.savefig(RESULTS_DIR / "badanie1" / "performance.png")
    plt.close()


def badanie2():
    """Study 2: Impact of initial data distribution"""
    distributions = ["random", "ascending", "descending", "part_33", "part_66"]
    size = E2_SIZE
    results = []

    for algo in ALGORITHMS:
        for dist in distributions:
            input_file = generate_input_file(size, dist, E2_DATA_TYPE)
            times = []
            for run_index in range(RUN_COUNT):
                time = run_algorithm("file", algo, E2_DATA_TYPE, input_file, run_index=run_index)
                times.append(time)

            results.append({
                "Algorithm": ALGORITHMS[algo],
                "Distribution": dist.replace("part_", ""),
                "Average": sum(times) / len(times),
                "Min": min(times),
                "Max": max(times),
                "Standard Deviation": pd.Series(times).std()
            })

    # Save and visualize
    output_file = RESULTS_DIR / "badanie2" / "results.csv"
    pd.DataFrame(results).to_csv(output_file, index=False)

    plt.figure(figsize=(12, 8))
    sns.barplot(x="Distribution", y="Average", hue="Algorithm", data=pd.DataFrame(results))
    plt.savefig(RESULTS_DIR / "badanie2" / "distribution_impact.png")
    plt.close()


def badanie3():
    """Study 3: Data type impact analysis"""
    selected_algorithm = E3_ALGORITHM_TO_TEST
    results = []

    for dtype in E3_DATA_TYPES:
        dtype_name = DATA_TYPES[dtype][0]
        times = []
        for run_index in range(RUN_COUNT):
            time = run_algorithm("test", selected_algorithm, dtype, E3_SIZE, run_index=run_index)
            times.append(time)

        results.append({
            "DataType": dtype_name,
            "Average": sum(times) / len(times),
            "Min": min(times),
            "Max": max(times),
            "Standard Deviation": pd.Series(times).std()
        })

    # Save and visualize
    output_file = RESULTS_DIR / "badanie3" / "results.csv"
    pd.DataFrame(results).to_csv(output_file, index=False)

    plt.figure(figsize=(12, 8))
    sns.barplot(x="DataType", y="Average", data=pd.DataFrame(results))
    plt.savefig(RESULTS_DIR / "badanie3" / "datatype_impact.png")
    plt.close()


if __name__ == "__main__":
    setup_directories()
    for study in EXPERIMENTS_TO_RUN:
        if study == 1:
            badanie1()
        elif study == 2:
            badanie2()
        elif study == 3:
            badanie3()
    print(f"All studies completed! Results saved in: '{RESULTS_DIR}'")
