
from pathlib import Path
import sys
import numpy as np
import blosc2


if __name__  == "__main__":
    assert len(sys.argv) == 2, "Usage: python create_test_files.py NUMBER_OF_FILES"

    cparams = {
        'codec': blosc2.Codec.ZSTD,
        'clevel': 8,
        'nthreads': 8
    }

    data = np.array([[0.1, 0.2], [0.3, 0.4]], dtype=np.float32)
    path_test = Path("test_files/test_simple.b2nd")
    if path_test.exists():
        path_test.unlink()
    blosc2.asarray(np.ascontiguousarray(data), urlpath=path_test, chunks=(1, 2), blocks=(1, 1), cparams=cparams)

    for i in range(int(sys.argv[1])):
        data = np.random.randn(480, 512, 512).astype(np.float32)

        # We only create the local fiels, the nfs files have to be copied to the server
        path = Path(f'test_files/test_real{i}.npy')
        path.parent.mkdir(parents=True, exist_ok=True)
        if path.exists():
            path.unlink()

        np.save(path, data)

        path = path.with_suffix(".b2nd")
        if path.exists():
            path.unlink()

        
        blosc2.asarray(np.ascontiguousarray(data), urlpath=path, chunks=(64, 64, 64), blocks=(16, 16, 16), cparams=cparams)
