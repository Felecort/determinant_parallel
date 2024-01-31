import os
import sys
import numpy as np
import pathlib
import pandas as pd

os.getcwd()
ROOT_FOLDER = os.getcwd() / pathlib.Path("../determinant")
"""
    [
        COMPILE_COMMAND,
        RUN_COMMAND,
        DELET OUTPUT,
    ]
"""
COMMANDS = {
    "omp": [f"gcc -DTEST -lm -fopenmp -o {ROOT_FOLDER/'out.omp'} {ROOT_FOLDER / 'det_omp.c'}",
            f"{ROOT_FOLDER/'out.omp'}",
            f"rm {ROOT_FOLDER/'out.omp'}"],

    "pthread": [f"gcc -DTEST -lm -lpthread -o {ROOT_FOLDER/'out.pthread'} {ROOT_FOLDER / 'det_pthread.c'}",
                f"{ROOT_FOLDER/'out.pthread'}",
                f"rm {ROOT_FOLDER/'out.pthread'}",],
    
    "mpi_c": [f"mpicc -DTEST -lm -o {ROOT_FOLDER/'out.mpi'} {ROOT_FOLDER / 'det_mpi.c'}",
              f"mpirun {ROOT_FOLDER/'out.mpi'} -np",
              f"rm {ROOT_FOLDER/'out.mpi'}",],
}


def check_result():
    df = pd.read_csv("generated_matrix.txt", sep=",", header=None)
    df.drop(columns=df.shape[0] , inplace=True)

    arr = np.array(df.values)
    det = np.linalg.det(arr)

    with open("result.txt", "r") as f:
        my_det = float(f.read())

    if my_det == 0:
        if det < 1e-19:
            ratio = 1
    else:
        ratio = np.abs(my_det / det)
    is_close = np.isclose(ratio, 1)
    print(f"RESULT: {is_close}\n")
    return is_close


if __name__ == "__main__":
    assert len(sys.argv) == 3, "Usage: python <realization> <proc|threads>\n\t\tAvailable realizations: mpi_c, omp, pthread"

    commands_for_test_object = COMMANDS[sys.argv[1]]
    max_calc_units = int(sys.argv[2])
    
    compilation, run, clear = commands_for_test_object
    
    is_test_passed = []
    # Compilation...
    os.system(compilation)
    for calc_units in range(1, max_calc_units + 1):
        for shape in [10, 100, 200, 400, 600, 800, 1000, 1200]:
            print(f"shape = {shape}, proc = {calc_units}")
            os.system(f"{run} {calc_units} {shape}")
            test_result = check_result()
            is_test_passed.append(test_result)
    print(f"\nNum tests: {len(is_test_passed)}\nPASSED: {sum(is_test_passed)}/{len(is_test_passed)}")
    os.system(clear) 



