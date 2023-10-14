# Benchmarks
Simple repository comparing the Raspberry Pi 4, Raspberry Pi IO Board compute module
and the NVIDIA Jetson Nano.

# TODO, add a table here with some result


Tests for RPI's and Jetson Nano:
* Sequential performance 
* CPU multithreading performance
* GPU multithreading
* SIMD(CPU) and cache performance
* SIMT(GPU) performance
* SIMD/SIMT + CPU/GPU multithreading performance

Tools used and needed:
* gprof for baseline benchmarks and code execution performance
* prof for flamegraphs
* NVCC for CUDA compilation
* g++ for C++ compilation
* `std::chrono` for naive time logging

Logic/algorithms implemented
* Matrix arithmetic (addition, subtraction, multiplication)
* Primality Tests & Generation
* TODO...



