import numpy as np
import pandas as pd
from time import time

if __name__ == "__main__":
    df = pd.read_csv("generated_matrix.txt", sep=",", header=None)
    df.drop(columns=df.shape[0] , inplace=True)

    arr = np.array(df.values)
    det = np.linalg.det(arr)

    with open("result.txt", "r") as f:
        my_det = float(f.read())
        print(my_det)

    ratio = np.abs(my_det / det)
    print("\n\nshape: ", arr.shape)
    print(f"DETERMINANT:", det)
    print(f"determinants ratio: {ratio:.3f}, Is close: {np.isclose(ratio, 1)}")