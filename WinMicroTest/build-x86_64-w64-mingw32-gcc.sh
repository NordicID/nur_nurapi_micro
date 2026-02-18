# packages: mingw-w64 cmake ninja-build
cmake -S . -B build -G Ninja -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc -DCMAKE_SYSTEM_NAME=Windows -DCMAKE_FIND_ROOT_PATH=/usr/x86_64-w64-mingw32 -DCMAKE_BUILD_TYPE=Release && cmake --build build --config Release
