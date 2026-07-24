run: build
  ./build/main

profile: build
    mkdir profiling
    nsys profile --trace=cuda,nvtx \
                 --cuda-um-cpu-page-faults=true \
                 --cuda-um-gpu-page-faults=true \
                 --force-overwrite=true \
                 --export=sqlite \
                 --output=profiling/profiling-result ./build/main

build: clean
    cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    cmake --build build/
    ln -sf build/compile_commands.json .

debug:
  cuda-gdb ./build/main

clean:
  rm -rf build/
  rm -rf compile_commands.json
  rm -rf profiling/

cloc:
    cloc include/ src/ CMakeLists.txt

stats:
    @echo "=== DEFAULT CUDA SUMMARY ==="
    nsys stats --report=cuda_api_sum,cuda_gpu_kern_sum,nvtx_sum --force-export=true profiling/profiling-result.nsys-rep
    @echo "=== UNIFIED MEMORY CPU FAULTS ==="
    nsys stats --report=um_cpu_page_faults_sum --force-export=true profiling/profiling-result.nsys-rep
    @echo "=== UNIFIED MEMORY GPU FAULTS ==="
    nsys stats --report=um_sum --force-export=true profiling/profiling-result.nsys-rep
    @echo "=== UNIFIED MEMORY MIGRATIONS ==="
    nsys stats --report=um_total_sum --force-export=true profiling/profiling-result.nsys-rep

copy:
    #!/usr/bin/env bash
    for f in CMakeLists.txt justfile $(find include src -type f); do
        echo "========================================"
        echo "FILE: $f"
        echo "========================================"
        cat "$f"
        echo -e "\n"
    done | wl-copy


