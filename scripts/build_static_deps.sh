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

CMAKE_EXTRA_FLAGS=""
BOOST_ARCH_ARGS=""
HOST_ARG=""

if [ "$(uname)" = "Darwin" ]; then
  ARCH="${CMAKE_OSX_ARCHITECTURES:-$(uname -m)}"
  export MACOSX_DEPLOYMENT_TARGET="${CMAKE_OSX_DEPLOYMENT_TARGET:-13.0}"
  export CFLAGS="${CFLAGS} -arch ${ARCH} -mmacosx-version-min=${MACOSX_DEPLOYMENT_TARGET}"
  export CXXFLAGS="${CXXFLAGS} -arch ${ARCH} -mmacosx-version-min=${MACOSX_DEPLOYMENT_TARGET}"
  CMAKE_EXTRA_FLAGS="-DCMAKE_OSX_ARCHITECTURES=${ARCH} -DCMAKE_OSX_DEPLOYMENT_TARGET=${MACOSX_DEPLOYMENT_TARGET}"
  if [ "${ARCH}" = "x86_64" ]; then
    BOOST_ARCH_ARGS="architecture=x86 address-model=64"
    HOST_ARG="--host=x86_64-apple-darwin"
  elif [ "${ARCH}" = "arm64" ]; then
    BOOST_ARCH_ARGS="architecture=arm address-model=64"
    HOST_ARG="--host=arm-apple-darwin"
  fi
fi

export PKG_CONFIG_PATH="${PREFIX}/lib/pkgconfig:${PREFIX}/lib64/pkgconfig:${PKG_CONFIG_PATH:-}"
export PATH="${PREFIX}/bin:${PATH}"

verify_sha() {
  local sha="$1"
  local file="$2"
  if command -v sha256sum >/dev/null 2>&1; then
    echo "${sha}  ${file}" | sha256sum -c -
  elif command -v shasum >/dev/null 2>&1; then
    echo "${sha}  ${file}" | shasum -a 256 -c -
  else
    python3 -c "import hashlib, sys; h=hashlib.sha256(open('${file}','rb').read()).hexdigest(); sys.exit(0 if h=='${sha}' else 1)"
  fi
}

fetch_and_verify() {
  local url="$1"
  local sha="$2"
  local out="$3"
  echo "==> Fetching ${url}..."
  curl -fsSL --retry 3 "${url}" -o "${out}"
  verify_sha "${sha}" "${out}"
}

# 1. zlib 1.3.1
ZLIB_URL="https://github.com/madler/zlib/releases/download/v1.3.1/zlib-1.3.1.tar.gz"
ZLIB_SHA="9a93b2b7dfdac77ceba5a558a580e74667dd6fede4585b91eefb60f03b72df23"
(
  cd "${BUILD_DIR}"
  fetch_and_verify "${ZLIB_URL}" "${ZLIB_SHA}" "zlib.tar.gz"
  tar -xzf "zlib.tar.gz"
  cd zlib-1.3.1
  CFLAGS="${CFLAGS}" ./configure --prefix="${PREFIX}" --static
  make -j"${NPROC}"
  make install
)

# 2. libpng 1.6.43
PNG_URL="https://download.sourceforge.net/libpng/libpng-1.6.43.tar.gz"
PNG_SHA="e804e465d4b109b5ad285a8fb71f0dd3f74f0068f91ce3cdfde618180c174925"
(
  cd "${BUILD_DIR}"
  fetch_and_verify "${PNG_URL}" "${PNG_SHA}" "libpng.tar.gz"
  tar -xzf "libpng.tar.gz"
  cd libpng-1.6.43
  CPPFLAGS="-I${PREFIX}/include" LDFLAGS="-L${PREFIX}/lib" ./configure --prefix="${PREFIX}" --enable-static --disable-shared ${HOST_ARG}
  make -j"${NPROC}"
  make install
)

# 3. libsndfile 1.2.2
SNDFILE_URL="https://github.com/libsndfile/libsndfile/releases/download/1.2.2/libsndfile-1.2.2.tar.xz"
SNDFILE_SHA="3799ca9924d3125038880367bf1468e53a1b7e3686a934f098b7e1d286cdb80e"
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
    -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
    ${CMAKE_EXTRA_FLAGS}
  cmake --build . -j"${NPROC}"
  cmake --install .
)

# 4. oneTBB v2021.13.0
TBB_URL="https://github.com/oneapi-src/oneTBB/archive/refs/tags/v2021.13.0.tar.gz"
TBB_SHA="3ad5dd08954b39d113dc5b3f8a8dc6dc1fd5250032b7c491eb07aed5c94133e1"
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
    -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
    ${CMAKE_EXTRA_FLAGS}
  cmake --build . -j"${NPROC}"
  cmake --install .
)

# 5. Boost 1.84.0
BOOST_URL="https://boostorg.jfrog.io/artifactory/main/release/1.84.0/source/boost_1_84_0.tar.bz2"
BOOST_SHA="71c32f4085e3adef4fffc90674a01079665d281d1de2aea6c6955db8567deae1"
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
    cxxflags="${CXXFLAGS}" \
    cflags="${CFLAGS}" \
    ${BOOST_ARCH_ARGS} \
    install
)

# 6. Armadillo 12.8.2 (header-only)
ARMA_URL="https://sourceforge.net/projects/arma/files/armadillo-12.8.2.tar.xz"
ARMA_SHA="03b62f8c09e4f5d74643b478520741b8e27b55e7e4525978fcae2f5d791ac3bf"
(
  cd "${BUILD_DIR}"
  fetch_and_verify "${ARMA_URL}" "${ARMA_SHA}" "armadillo.tar.xz"
  tar -xf "armadillo.tar.xz"
  mkdir -p "${PREFIX}/include"
  cp -r armadillo-12.8.2/include/* "${PREFIX}/include/"
)

echo "==> Successfully installed all static dependencies to ${PREFIX}!"
