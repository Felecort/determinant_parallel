import numpy as np
import pandas as pd

if __name__ == "__main__":
    df = pd.read_csv("generated_matrix.txt", sep=",", header=None)
    df.drop(columns=df.shape[0] , inplace=True)

    arr = np.array(df.values)

    det = np.linalg.det(arr)

    print(f"DETERMINANT:", det)