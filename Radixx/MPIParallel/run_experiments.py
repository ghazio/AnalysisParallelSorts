import csv
import platform
import re
import subprocess
from pathlib import Path

THREAD_COUNTS = [1, 2, 4, 8, 16, 32]
BOARD_SIZES = [64, 128, 256, 512, 1024, 2048]
NUM_SAMPLES = 5

HOSTNAME = platform.node()
SCRIPT_DIR = Path(__file__).parent
DATA_DIR = SCRIPT_DIR / 'data'

DATA_DIR.mkdir(exist_ok=True)

def parse_output(output):
    runtime = re.search('Total time: (.*) seconds', output)
    if runtime:
        return float(runtime.group(1))

def run_gol(thread_count, size, iterations, partition_columns):
    args = [
        SCRIPT_DIR / '../gol',
        "-t", str(thread_count),
        "-n", str(size),
        "-m", str(size),
        "-k", str(iterations),
        "-x"
    ]

    if partition_columns:
        args.append("-c")

    output = subprocess.run(args, capture_output=True)

    return parse_output(str(output.stdout))

threads_and_size_file = DATA_DIR / f'{HOSTNAME}_threads_and_size.csv'
print(threads_and_size_file)
with open(threads_and_size_file, 'w+', newline='') as f:
    fieldnames = ['thread_count', 'size', 'runtime']
    writer = csv.DictWriter(f, fieldnames=fieldnames)
    writer.writeheader()

    for thread_count in THREAD_COUNTS:
        for size in BOARD_SIZES:
            for i in range(NUM_SAMPLES):
                runtime = run_gol(thread_count, size, 1000, False)
                print(thread_count, size, runtime)
                writer.writerow({
                    'thread_count': thread_count,
                    'size': size,
                    'runtime': runtime
                })

threads_and_partition_mode_file = DATA_DIR / f'{HOSTNAME}_threads_and_partition_mode.csv'
print(threads_and_partition_mode_file)
with open(threads_and_partition_mode_file, 'w+', newline='') as f:
    fieldnames = ['thread_count', 'partition_columns', 'runtime']
    writer = csv.DictWriter(f, fieldnames=fieldnames)
    writer.writeheader()

    for thread_count in THREAD_COUNTS:
        for partition_columns in [True, False]:
            for i in range(NUM_SAMPLES):
                runtime = run_gol(thread_count, 1024, 1000, partition_columns)
                print(thread_count, partition_columns, runtime)
                writer.writerow({
                    'thread_count': thread_count,
                    'partition_columns': partition_columns,
                    'runtime': runtime
                })
