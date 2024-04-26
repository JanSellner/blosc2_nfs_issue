import os
import sys

if __name__  == "__main__":
    os.environ["MKL_NUM_THREADS"] = "1"
    os.environ["NUMEXPR_NUM_THREADS"] = "1"
    os.environ["OMP_NUM_THREADS"] = "1"
    import numpy as np

    assert len(sys.argv) == 2, "Usage: python main.py FILE"
    data = np.load(sys.argv[1], mmap_mode='r')
    crop = data[100:228, 1:129, 100:228].copy()
