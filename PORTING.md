# PhaseLimiter Portable Fork

This repository is a portable fork of [ai-mastering/phaselimiter](https://github.com/ai-mastering/phaselimiter) modernized to build and run across modern architectures and platforms without proprietary dependencies.

## Key Modernizations

1. **Intel IPP Removal & PocketFFT Integration**:
   - Replaced proprietary Intel IPP dependency with an open-source IPP compatibility layer (`deps/bakuage/include/ipp_compat/ipp.h` and `deps/bakuage/src/ipp_compat.cpp`).
   - Integrated [PocketFFT](https://gitlab.mpcdf.mpg.de/mtr/pocketfft) (BSD-3-Clause header-only) supporting 1D and 2D real and complex DFT/FFT transforms in IPP Perm and CCS formats.
   - Preserves numerical equivalence against upstream Intel IPP gold binaries:
     - **Linux x86_64**: Peak diff 0.0008 dB, RMS diff 0.0000 dB, RMS difference signal -97.97 dBFS.
     - **macOS Apple Silicon arm64**: Peak diff 0.0003 dB, RMS diff 0.0000 dB, RMS difference signal -98.60 dBFS.
   - Intel IPP remains optional via `-DBAKUAGE_USE_IPP=ON`.

2. **Apple Silicon & ARM64 Native Port**:
   - Patched libsimdpp NEON 64-bit hardware vector square root (`_sqrtq_f32`), resolving NaN propagation during gradient optimization on ARM architectures.
   - Ported x86 intrinsics in FIR filtering and HNSW libraries to portable ARM NEON / scalar fallback.
   - Auto-patches submodules at CMake configure time for out-of-the-box building on any CPU architecture.

3. **oneTBB 2021+ Compatibility**:
   - Replaced deprecated `tbb/pipeline.h` and legacy task scheduler APIs (`tbb::task_scheduler_init`) with modern `tbb::global_control`.
   - Updated memory allocator invocations in `GradCalculator.h` to use `std::allocator_traits`.
   - Updated pipeline filter modes in `audio_visualizer` to modern `tbb::filter_mode`.

4. **Compiler & Standard Modernization**:
   - GCC 15 and Apple Clang 21+ compatible (resolved NULL-to-pointer conversions and missing `<cstdint>` headers).
   - Modernized CMake build system with standard `find_package` for Boost, oneTBB, libsndfile, Armadillo, PNG, and ZLIB.
   - SIMD architecture auto-detection: AVX/AVX2/FMA3 on x86_64, NEON on ARM64.

5. **In-Tree WAV I/O & Zero External Audio Library Dependency (v0.2.0-native3)**:
   - Replaced LGPL-licensed `libsndfile` dependency with an in-tree WAV reader and writer (`deps/bakuage/include/bakuage/wav_io.h` and `deps/bakuage/src/wav_io.cpp`).
   - Supports 16-bit, 24-bit, 32-bit signed integer PCM and 32-bit floating point PCM WAV files.
   - Controlled via CMake option `-DPHASELIMITER_WAV_ONLY=ON` (enabled by default).
   - Eliminates `libsndfile` from runtime and static build dependencies.
   - Third-party licenses for all static and bundled dependencies are explicitly tracked in `licenses/` and packaged with all binary distribution tarballs.

## Upstream runtime dependencies

Inspected via `upx -d` and `readelf -d bin/phase_limiter | grep NEEDED` on upstream `release.tar.xz`:

```text
 0x0000000000000001 (NEEDED)             Shared library: [libsndfile.so.1]
 0x0000000000000001 (NEEDED)             Shared library: [libtbb.so.2]
 0x0000000000000001 (NEEDED)             Shared library: [libtbbmalloc.so.2]
 0x0000000000000001 (NEEDED)             Shared library: [libtbbmalloc_proxy.so.2]
 0x0000000000000001 (NEEDED)             Shared library: [libpthread.so.0]
 0x0000000000000001 (NEEDED)             Shared library: [libstdc++.so.6]
 0x0000000000000001 (NEEDED)             Shared library: [libm.so.6]
 0x0000000000000001 (NEEDED)             Shared library: [libgcc_s.so.1]
 0x0000000000000001 (NEEDED)             Shared library: [libc.so.6]
```

## Prerequisites

### Ubuntu / Debian (x86_64 & ARM64)
```bash
sudo apt update
sudo apt install -y build-essential cmake pkg-config \
    libboost-filesystem-dev libboost-serialization-dev libboost-iostreams-dev \
    libtbb-dev libarmadillo-dev libpng-dev zlib1g-dev ffmpeg
```
*(Note: `libsndfile1-dev` is only required if building legacy tools with `-DPHASELIMITER_WAV_ONLY=OFF`)*

### macOS (Apple Silicon & Intel)
```bash
brew install cmake pkg-config boost tbb armadillo libpng ffmpeg
```
*(Note: `libsndfile` is only required if building legacy tools with `-DPHASELIMITER_WAV_ONLY=OFF`)*

## Build Instructions

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DPHASELIMITER_WAV_ONLY=ON
cmake --build . --target phase_limiter -j2
```

To run the test suite:
```bash
cmake --build . --target test_bin -j2
./bin/test_bin
```
