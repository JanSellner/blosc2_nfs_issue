from functools import partial
import multiprocessing.pool as mpp
from pathlib import Path
import subprocess
import sys


def process_file_npy(i: int, file: Path) -> None:
    file = file.with_stem(file.stem + str(i))
    res = subprocess.run([sys.executable, "main.py", file])
    assert res.returncode == 0, f"Error processing file {file}"


def process_file_b2nd(i: int, mode: str, file: Path) -> None:
    file = file.with_stem(file.stem + str(i))
    res = subprocess.run(["build/blosc_mmap", mode, file])
    assert res.returncode == 0, f"Error processing file {file}"


if __name__  == "__main__":
    assert len(sys.argv) == 2 or len(sys.argv) == 3, "Usage: python main_mp.py [default|mmap] FILE"

    if sys.argv[1] in ["default", "mmap"]:
        mode = sys.argv[1]
        file = Path(sys.argv[2])
        assert file.suffix == ".b2nd", "Only .b2nd files are supported when providing two arguments"
    else:
        file = Path(sys.argv[1])
        assert file.suffix == ".npy", "Only .npy files are supported when providing one argument"

    n_processes = 8
    pool = mpp.Pool(n_processes)

    if file.suffix == ".npy":
        pool.map(partial(process_file_npy, file=file), range(n_processes))
    elif file.suffix == ".b2nd":
        pool.map(partial(process_file_b2nd, file=file, mode=mode), range(n_processes))

    pool.close()
    pool.join()
