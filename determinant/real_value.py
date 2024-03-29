import numpy as np
import pandas as pd


if __name__ == "__main__":
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
    print("\n\nTEST USING numpy")
    print(f"DETERMINANT:", det)
    print(f"determinants ratio: {ratio:.3f}")
    print("\n---RESULT---")
    print(f"Is close: {np.isclose(ratio, 1)}")