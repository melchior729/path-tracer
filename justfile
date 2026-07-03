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
    #!/usr/bin/env bash
    for f in CMakeLists.txt justfile $(find include src -type f); do
        echo "========================================"
        echo "FILE: $f"
        echo "========================================"
        cat "$f"
        echo -e "\n"
    done | wl-copy

cloc:
    cloc include/ src/ CMakeLists.txt

