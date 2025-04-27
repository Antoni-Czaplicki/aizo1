import enum
import subprocess


class SortingAlgorithm(enum.Enum):
    INSERTION = "0"
    HEAP = "1"
    SHELL = "2"
    QUICK = "3"
    DRUNK_QUICK = "4"


class DataType(enum.Enum):
    INT = "0"
    FLOAT = "1"
    STRING = "2"
    BOARDGAME = "3"


sum = 0
for i in range(100):
    sum += int(subprocess.check_output(
        ["cmake-build-debug/aizo1", "--test", SortingAlgorithm.INSERTION.value, DataType.INT.value, "10000"]).decode(
        "utf-8"))

print("Sum of all tests: ", sum)
print("Average of all tests: ", sum / 10000)
