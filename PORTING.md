# PhaseLimiter Portable Fork

This repository is a portable fork of [ai-mastering/phaselimiter](https://github.com/ai-mastering/phaselimiter) modernized to build and run across modern architectures and platforms without proprietary dependencies.

## Key Modernizations

1. **Intel IPP Removal & PocketFFT Integration**:
   - Replaced proprietary Intel IPP dependency with an open-source IPP compatibility layer (`deps/bakuage/include/ipp_compat/ipp.h` and `deps/bakuage/src/ipp_compat.cpp`).
   - Integrated [PocketFFT](https://gitlab.mpcdf.mpg.de/mtr/pocketfft) (BSD-3-Clause header-only) supporting 1D and 2D real and complex DFT/FFT transforms in IPP Perm and CCS formats.
   - Preserves numerical equivalence: RMS difference against upstream IPP binary is -97.97 dBFS (< 0.0001 dB difference).
   - Intel IPP remains optional via `-DBAKUAGE_USE_IPP=ON`.

2. **oneTBB 2021+ Compatibility**:
   - Replaced deprecated `tbb/pipeline.h` and legacy task scheduler APIs (`tbb::task_scheduler_init`) with modern `tbb::global_control`.
   - Updated memory allocator invocations in `GradCalculator.h` to use `std::allocator_traits`.
   - Updated pipeline filter modes in `audio_visualizer` to modern `tbb::filter_mode`.

3. **Compiler & Standard Modernization**:
   - GCC 15 and Clang 19+ compatible (resolved NULL-to-pointer conversions and missing `<cstdint>` headers).
   - Modernized CMake build system with standard `find_package` for Boost, oneTBB, libsndfile, Armadillo, PNG, and ZLIB.
   - SIMD architecture auto-detection: AVX/AVX2/FMA3 on x86_64, NEON on ARM64.

## Prerequisites

### Ubuntu / Debian (x86_64 & ARM64)
```bash
sudo apt update
sudo apt install -y build-essential cmake pkg-config \
    libboost-filesystem-dev libboost-serialization-dev libboost-iostreams-dev \
    libtbb-dev libsndfile1-dev libarmadillo-dev libpng-dev zlib1g-dev ffmpeg
```

### macOS (Apple Silicon & Intel)
```bash
brew install cmake pkg-config boost tbb libsndfile armadillo libpng ffmpeg
```

## Build Instructions

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --target phase_limiter -j4
```

To run the test suite:
```bash
cmake --build . --target test_bin -j4
./bin/test_bin
```
