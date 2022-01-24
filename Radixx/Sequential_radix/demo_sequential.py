import csv
import platform
import re
import os
import time
import subprocess
from pathlib import Path

NUM_SAMPLES = 5

NUM_SAMPLES = 5

HOSTNAME   = platform.node()
SCRIPT_DIR = os.getcwd()
DATA_DIR   = os.path.join(SCRIPT_DIR, 'data')
if os.path.isfile(DATA_DIR) == True:
    os.mkdir(DATA_DIR)

myfile     = 'radix'
executeable = os.path.join(SCRIPT_DIR, myfile)

def parse_output(output):
    runtime = re.search('Total time: (.*) seconds', output)
    if runtime:
        return float(runtime.group(1))

def run_sort(size):
    start_time = time.time()
    args = ['time', executeable, str(size)]
    code = subprocess.run(args, capture_output=True)
    execution_time = time.time()-start_time
    print(code)
    print(execution_time)
    return execution_time

size = 500;
filename=f'{HOSTNAME}__size.csv'
size_file =os.path.join(DATA_DIR, filename)
print(size_file)
with open(size_file, 'w+', newline='') as f:
    fieldnames = ['size', 'runtime']
    writer = csv.DictWriter(f, fieldnames=fieldnames)
    writer.writeheader()

    while (size<500000):
        filename=f'{HOSTNAME}__size.csv'

        runtime = run_sort(size)
        print(size, runtime)
        writer.writerow({
            'size': size,
            'runtime': runtime
            })
        size *= 2
