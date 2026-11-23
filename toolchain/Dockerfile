# Project build container: clang 5.0.1, the LLVM release the whole binary was
# built with (per the DWARF producer strings in the .nss; see the README,
# "Build"). It is installed at /opt/llvm, which PATH points at, so `clang++`,
# `ld.lld`, `llvm-nm` etc. are 5.0.1's.
#
# The release is a Linux-only prebuilt, which is why the CMake configure/build
# happens in here rather than natively on a macOS host.
# The image is architecture-parametric.  D3_DOCKER_PLATFORM (default
# linux/arm64, what the Mac uses) picks the base platform, and LLVM_ARCH -- the
# architecture part of releases.llvm.org's tarball names -- is derived from it
# below; an explicit value must match the lock.  Set D3_DOCKER_PLATFORM=linux/amd64 on an
# x86_64 host to get the same release as an x86_64-linux-gnu-ubuntu-16.04
# build; the compiler is a cross-compiler either way (--target=aarch64-
# linux-gnu in toolchain/ToolchainNX64.cmake).
ARG D3_DOCKER_PLATFORM=linux/arm64
FROM --platform=${D3_DOCKER_PLATFORM} ubuntu:20.04
ARG D3_DOCKER_PLATFORM=linux/arm64

ARG CLANG_VERSION=5.0.1
ENV CLANG_VERSION=${CLANG_VERSION}

# Empty overrides select the locked official archive. Nonmatching overrides fail.
# LLVM_ARCH empty means "derive from D3_DOCKER_PLATFORM":
#   linux/arm64 -> aarch64-linux-gnu               (the Mac default)
#   linux/amd64 -> x86_64-linux-gnu-ubuntu-16.04
ARG LLVM_ARCH=
ARG CLANG_URL=

RUN apt-get update && apt-get install -y --no-install-recommends \
        python3 xz-utils ca-certificates libtinfo5 libc6-dev \
        cmake ninja-build lld \
    && rm -rf /var/lib/apt/lists/*

# The compile is --target=aarch64-linux-gnu, so clang wants glibc's aarch64
# headers at /usr/include/aarch64-linux-gnu.  On the arm64 image the native
# libc6-dev already put them there; on any other host architecture install
# the very same package for arm64 through multiarch (Ubuntu's arm64 packages
# live on ports.ubuntu.com), at the same glibc version, so the header set
# the compiler sees is identical.  The link is a bare `ld.lld -shared`
# (CMakeLists.txt), so no aarch64 libraries are needed.
RUN set -e; case "${D3_DOCKER_PLATFORM}" in linux/arm64*) exit 0 ;; esac; \
    dpkg --add-architecture arm64 \
    && sed -i 's/^deb /deb [arch=amd64] /' /etc/apt/sources.list \
    && . /etc/os-release \
    && for s in "$VERSION_CODENAME" "$VERSION_CODENAME-updates" "$VERSION_CODENAME-security"; do \
           echo "deb [arch=arm64] http://ports.ubuntu.com/ubuntu-ports $s main universe"; \
       done > /etc/apt/sources.list.d/arm64-ports.list \
    && apt-get update && apt-get install -y --no-install-recommends libc6-dev:arm64 \
    && rm -rf /var/lib/apt/lists/* \
    && test -f /usr/include/aarch64-linux-gnu/sys/cdefs.h

# One lock and verifier cover fresh and cached archives, before tar can run.
# The verifier never executes downloaded content. No checksum/URL skip option.
COPY Dockerfile llvm.lock.json llvm_archive.py image_provenance.py /opt/d3-toolchain/
RUN set -eu; archive="$(python3 /opt/d3-toolchain/llvm_archive.py \
        --platform "${D3_DOCKER_PLATFORM}" --version "${CLANG_VERSION}" \
        --arch "${LLVM_ARCH}" --url "${CLANG_URL}" \
        --cache-dir /tmp/llvm-downloads --allow-downloads)" \
    && mkdir -p "/opt/llvm-${CLANG_VERSION}" \
    && tar -xJf "$archive" -C "/opt/llvm-${CLANG_VERSION}" --strip-components=1 \
    && ln -s "/opt/llvm-${CLANG_VERSION}" /opt/llvm \
    && python3 /opt/d3-toolchain/image_provenance.py --record \
    && rm -rf /tmp/llvm-downloads

ENV PATH="/opt/llvm/bin:${PATH}"

WORKDIR /work
