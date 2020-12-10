#!/usr/bin/python3
import sys
import pandas as pd
import numpy as np

def print_help():
    print("EXECUTION: python3 this_script.py csvfile.csv")
    print(".csv file ideally came from test_time.sh, but "
        "if it didn't, here's an example: \n"
        "\nSource;Threads;Input;Result;Output\n"
        "sequential;1;10x1M;1;ok\n"
        "sequential;1;10x1M;1;ok\n"
        "parallel;1;10x1M;2;ok\n"
        "parallel;1;10x1M;2;ok\n"
        "sequential_percentage;1;10x1M;3;ok\n"
        "sequential_percentage;1;10x1M;3;ok\n"
        "parallel_percentage;1;10x1M;4;ok\n"
        "parallel_percentage;1;10x1M;4;ok\n"
        "\nFor more information on what each column means, "
        "run bash test_time.sh --help"
        )

if len(sys.argv) == 1 or sys.argv[1] == "--help":
    print_help()
else:
    df = pd.read_csv(sys.argv[1], delimiter=';')
    df['Output'] = df['Output'].apply(lambda x: x == 'ok')

    if len(df[df['Output'] == False]):
        print("Some outputs are wrong:")
        print(df[df['Output'] == False])
        print("\nEnding execution (wrong outputs)")
        sys.exit()

    grouped_df = df.drop('Output', axis=1).groupby(['Source', 'Threads', 'Input'])

    print("MEAN VALUES")
    print(grouped_df.mean())
    print("\n\nSTD DEVIATION VALUES")
    print(grouped_df.std())
