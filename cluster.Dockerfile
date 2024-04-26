FROM blosc2

COPY . /workspaces/blosc_mmap
WORKDIR /workspaces/blosc_mmap

# build blsoc (cmake commands from vscode)
RUN mkdir build \
 && /usr/bin/cmake --no-warn-unused-cli -DCMAKE_BUILD_TYPE:STRING=Release -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_C_COMPILER:FILEPATH=/usr/bin/gcc -DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/g++ -S/workspaces/blosc_mmap -B/workspaces/blosc_mmap/build -G "Unix Makefiles" \
 && /usr/bin/cmake --build /workspaces/blosc_mmap/build --config Release --target all -j 34 --
