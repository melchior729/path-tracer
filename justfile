run: build
  ./build/main

build:
    cmake -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    cmake --build build/
    ln -sf build/compile_commands.json .

debug:
  gdb ./build/main

clean:
  rm -rf build/

copy:
  cat include/* src/* | wl-copy

