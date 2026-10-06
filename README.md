# PhaseLimiter Portable

Upstream PhaseLimiter had an effective mastering algorithm, but you could not build it on modern computers.

The original project stopped updates in 2019. It required proprietary Intel IPP libraries, ran only on x86, and depended on abandoned build toolchains.

This fork fixes that. We stripped out proprietary dependencies, ported the engine to modern 64-bit ARM, and verified numerical audio equivalence against Intel reference binaries.

Now PhaseLimiter builds and runs natively on Apple Silicon, modern Linux, and x86_64.

---

## What Changed

- **Removed Intel IPP:** We replaced Intel's proprietary IPP library with an open-source PocketFFT compatibility layer.
- **Apple Silicon & ARM64 Support:** We fixed ARM vector math bugs and ported x86 SIMD intrinsics to ARM NEON.
- **Verified Numerical Equivalence:** Automated CI runs both versions on identical audio. Peak difference stays under 0.0008 dB.
- **In-Tree WAV Engine:** We eliminated libsndfile and its dynamic library dependency. PhaseLimiter reads and writes uncompressed WAV files directly.
- **Modern C++ Toolchains:** The codebase compiles cleanly on GCC 15 and Clang 21 with oneTBB 2021+.

---

## Download Prebuilt Binaries

GitHub Actions builds release tarballs on every release:

- [Linux x86_64](https://github.com/soficis/phaselimiter/releases/latest)
- [Linux ARM64](https://github.com/soficis/phaselimiter/releases/latest)
- [macOS Apple Silicon (M1/M2/M3/M4)](https://github.com/soficis/phaselimiter/releases/latest)
- [macOS Intel](https://github.com/soficis/phaselimiter/releases/latest)

---

## Quickstart

Run PhaseLimiter directly against any audio file:

```bash
phase_limiter --input input.wav --output output.wav
```

---

## Build from Source

### 1. Install Dependencies

**Ubuntu / Debian (x86_64 & ARM64):**
```bash
sudo apt update
sudo apt install -y build-essential cmake pkg-config \
    libboost-filesystem-dev libboost-serialization-dev libboost-iostreams-dev \
    libtbb-dev libarmadillo-dev libpng-dev zlib1g-dev ffmpeg
```

**macOS (Homebrew):**
```bash
brew install cmake pkg-config boost tbb armadillo libpng ffmpeg
```

### 2. Compile

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DPHASELIMITER_WAV_ONLY=ON
cmake --build . --target phase_limiter -j2
```

### 3. Run Tests

```bash
cmake --build . --target test_bin -j2
./bin/test_bin
```

---

## Numerical Equivalence Gate

We measure every release against upstream Intel IPP reference output:

| Platform | Peak Diff | RMS Diff | Difference Signal |
| :--- | :--- | :--- | :--- |
| **Linux x86_64** | 0.0008 dB | 0.0000 dB | -97.97 dBFS |
| **macOS Apple Silicon** | 0.0003 dB | 0.0000 dB | -98.60 dBFS |

The difference sits far below human hearing thresholds.

---

## License

MIT License. See [LICENSE](LICENSE) for details. Third-party licenses for bundled dependencies live in licenses/.
