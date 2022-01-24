import pandas as pd


def main():
    size = []
    runtime_array = []
    desired_file = input("Enter the file you wish to parse: ")
    with open(desired_file, 'r') as fp:
        lines = fp.readlines()
    for line in lines:
        size.append(line[0:6])
        runtime_array.append(line[7:])
        
    print(size)   
main()