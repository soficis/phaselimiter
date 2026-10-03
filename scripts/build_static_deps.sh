#!/usr/bin/env bash
set -euo pipefail

if [ $# -lt 1 ]; then
  echo "Usage: $0 <prefix>"
  exit 1
fi

PREFIX="$(cd "$1" 2>/dev/null && pwd || (mkdir -p "$1" && cd "$1" && pwd))"
BUILD_DIR="$(mktemp -d -t phaselimiter-deps-build-XXXXXX)"
trap 'rm -rf "${BUILD_DIR}"' EXIT

NPROC=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 2)

export CFLAGS="-fPIC -O2"
export CXXFLAGS="-fPIC -O2"
if [ "$(uname)" = "Darwin" ]; then
  export MACOSX_DEPLOYMENT_TARGET="${CMAKE_OSX_DEPLOYMENT_TARGET:-13.0}"
  export CFLAGS="${CFLAGS} -mmacosx-version-min=${MACOSX_DEPLOYMENT_TARGET}"
  export CXXFLAGS="${CXXFLAGS} -mmacosx-version-min=${MACOSX_DEPLOYMENT_TARGET}"
fi

export PKG_CONFIG_PATH="${PREFIX}/lib/pkgconfig:${PREFIX}/lib64/pkgconfig:${PKG_CONFIG_PATH:-}"
export PATH="${PREFIX}/bin:${PATH}"

fetch_and_verify() {
  local url="$1"
  local sha="$2"
  local out="$3"
  echo "==> Fetching ${url}..."
  curl -fsSL --retry 3 "${url}" -o "${out}"
  echo "${sha}  ${out}" | sha256sum -c -
}

# 1. zlib 1.3.1
ZLIB_URL="https://github.com/madler/zlib/releases/download/v1.3.1/zlib-1.3.1.tar.gz"
ZLIB_SHA="9a93b2b7dfdac77ceba5a558a580e74667dd6fede4585b91eefb60f03b72df23"
(
  cd "${BUILD_DIR}"
  fetch_and_verify "${ZLIB_URL}" "${ZLIB_SHA}" "zlib.tar.gz"
  tar -xzf "zlib.tar.gz"
  cd zlib-1.3.1
  ./configure --prefix="${PREFIX}" --static
  make -j"${NPROC}"
  make install
)

# 2. libpng 1.6.43
PNG_URL="https://download.sourceforge.net/libpng/libpng-1.6.43.tar.gz"
PNG_SHA="6a5ca0652392a2d7c96b0cfa02aec7049e67972ba3520e44754da74c016f731a"
(
  cd "${BUILD_DIR}"
  fetch_and_verify "${PNG_URL}" "${PNG_SHA}" "libpng.tar.gz"
  tar -xzf "libpng.tar.gz"
  cd libpng-1.6.43
  CPPFLAGS="-I${PREFIX}/include" LDFLAGS="-L${PREFIX}/lib" ./configure --prefix="${PREFIX}" --enable-static --disable-shared
  make -j"${NPROC}"
  make install
)

# 3. libsndfile 1.2.2
SNDFILE_URL="https://github.com/libsndfile/libsndfile/releases/download/1.2.2/libsndfile-1.2.2.tar.xz"
SNDFILE_SHA="3799ca9924d31250388803da1d001e634d873d32fa5a239b49f7fa6557330be6"
(
  cd "${BUILD_DIR}"
  fetch_and_verify "${SNDFILE_URL}" "${SNDFILE_SHA}" "libsndfile.tar.xz"
  tar -xf "libsndfile.tar.xz"
  mkdir libsndfile-1.2.2/build && cd libsndfile-1.2.2/build
  cmake .. \
    -DCMAKE_INSTALL_PREFIX="${PREFIX}" \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_SHARED_LIBS=OFF \
    -DBUILD_PROGRAMS=OFF \
    -DBUILD_EXAMPLES=OFF \
    -DBUILD_TESTING=OFF \
    -DENABLE_EXTERNAL_LIBS=OFF \
    -DENABLE_MPEG=OFF \
    -DCMAKE_POSITION_INDEPENDENT_CODE=ON
  cmake --build . -j"${NPROC}"
  cmake --install .
)

# 4. oneTBB v2021.13.0
TBB_URL="https://github.com/oneapi-src/oneTBB/archive/refs/tags/v2021.13.0.tar.gz"
TBB_SHA="57657d42cfd95cf233959b85934446cbbeab255f0562c5cb4e81561f5d8e7da3"
(
  cd "${BUILD_DIR}"
  fetch_and_verify "${TBB_URL}" "${TBB_SHA}" "tbb.tar.gz"
  tar -xzf "tbb.tar.gz"
  mkdir oneTBB-2021.13.0/build && cd oneTBB-2021.13.0/build
  cmake .. \
    -DCMAKE_INSTALL_PREFIX="${PREFIX}" \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_SHARED_LIBS=OFF \
    -DTBB_TEST=OFF \
    -DTBB_STRICT=OFF \
    -DCMAKE_POSITION_INDEPENDENT_CODE=ON
  cmake --build . -j"${NPROC}"
  cmake --install .
)

# 5. Boost 1.84.0
BOOST_URL="https://boostorg.jfrog.io/artifactory/main/release/1.84.0/source/boost_1_84_0.tar.bz2"
BOOST_SHA="cc4b893acf645399d1057963261d94a13ea9f6b84da0ea11284021fb9e8e6766"
(
  cd "${BUILD_DIR}"
  fetch_and_verify "${BOOST_URL}" "${BOOST_SHA}" "boost.tar.bz2"
  tar -xjf "boost.tar.bz2"
  cd boost_1_84_0
  ./bootstrap.sh --prefix="${PREFIX}" --with-libraries=filesystem,iostreams,serialization
  ./b2 -j"${NPROC}" \
    --prefix="${PREFIX}" \
    link=static \
    variant=release \
    threading=multi \
    runtime-link=shared \
    cxxflags="-fPIC -O2" \
    install
)

# 6. Armadillo 12.8.2 (header-only)
ARMA_URL="https://sourceforge.net/projects/arma/files/armadillo-12.8.2.tar.xz"
ARMA_SHA="1c8930438cf112ff7ce3c9657091faec8b89e3ecdd820bc0a8677c7b8d4f0bf9"
(
  cd "${BUILD_DIR}"
  fetch_and_verify "${ARMA_URL}" "${ARMA_SHA}" "armadillo.tar.xz"
  tar -xf "armadillo.tar.xz"
  mkdir -p "${PREFIX}/include"
  cp -r armadillo-12.8.2/include/* "${PREFIX}/include/"
)

echo "==> Successfully installed all static dependencies to ${PREFIX}!"
