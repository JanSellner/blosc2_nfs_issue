# BLOSC2 Benchmarking
The purpose of this repository is to investigate the performance bottleneck of blosc2 which can happen when partially reading files from NFS shares ([issue #188](https://github.com/Blosc/python-blosc2/issues/188)).

## Preparation
The NFS mount is supposed to be mounted at `/mnt/blosc_test_files` and should contain the same test files as used locally (generated via `python create_test_files.py 8`). To get started, start vscode and run the `Dev Containers: Open Folder in Container...` command from the Command Palette. This will build and start the Docker container `blosc2`.

## Running the benchmark
Compile the binaries via `cmake` by right-clicking on `CMakeLists.txt` and select `Build All Projects`. Make sure the release config is used.

After compilation, run the benchmark via (inside the Docker container: `docker exec -it blosc2 bash`):
```bash
# Serial local
hyperfine --warmup 1 --prepare 'echo 3 > /writable_proc/sys/vm/drop_caches' --parameter-list COMMAND "build/blosc_mmap default test_files/test_real0.b2nd,build/blosc_mmap mmap test_files/test_real0.b2nd,python main.py test_files/test_real0.npy" "{COMMAND}"

# Serial network
hyperfine --warmup 1 --prepare 'echo 3 > /writable_proc/sys/vm/drop_caches' --parameter-list COMMAND "build/blosc_mmap default test_files_nfs/test_real0.b2nd,build/blosc_mmap mmap test_files_nfs/test_real0.b2nd,python main.py test_files_nfs/test_real0.npy" "{COMMAND}"

# Serial network with caching
hyperfine --warmup 3 --parameter-list COMMAND "build/blosc_mmap default test_files_nfs/test_real0.b2nd,build/blosc_mmap mmap test_files_nfs/test_real0.b2nd,python main.py test_files_nfs/test_real0.npy" "{COMMAND}"

# Multiprocessing local
hyperfine --warmup 1 --prepare 'echo 3 > /writable_proc/sys/vm/drop_caches' --parameter-list COMMAND "python main_mp.py default test_files/test_real.b2nd,python main_mp.py mmap test_files/test_real.b2nd,python main_mp.py test_files/test_real.npy" "{COMMAND}"

# Multiprocessing network
hyperfine --warmup 1 --prepare 'echo 3 > /writable_proc/sys/vm/drop_caches' --parameter-list COMMAND "python main_mp.py default test_files_nfs/test_real.b2nd,python main_mp.py mmap test_files_nfs/test_real.b2nd,python main_mp.py test_files_nfs/test_real.npy" "{COMMAND}"
```
