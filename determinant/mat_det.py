import numpy as np
import pandas as pd
from time import time

if __name__ == "__main__":
    df = pd.read_csv("generated_matrix.txt", sep=",", header=None)
    df.drop(columns=df.shape[0] , inplace=True)

    arr = np.array(df.values)
    start = time()
    det = np.linalg.det(arr)
    stop = time()

    print("\n\nshape: ", arr.shape)
    print(f"DETERMINANT:", det)
    print("Time:", stop - start)