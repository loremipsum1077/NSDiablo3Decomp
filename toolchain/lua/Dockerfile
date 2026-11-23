# Keep the project's frontend, headers and linker; add only backend build tools.
ARG BASE_IMAGE=d3-build:5.0.1
FROM ${BASE_IMAGE}
RUN apt-get update && apt-get install -y --no-install-recommends gcc g++ python3 patch \
    && rm -rf /var/lib/apt/lists/*
