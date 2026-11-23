"""Build Google's 2.6.0 protoc in the local tool cache from the pinned public release."""
import hashlib
import os
from pathlib import Path
import shutil
import subprocess
import tarfile
from tempfile import TemporaryDirectory
from urllib.request import urlopen

URL = "https://github.com/protocolbuffers/protobuf/releases/download/v2.6.0/protobuf-2.6.0.tar.bz2"
SHA256 = "0a2f8533b2e0587a2b4efce0c4c8aea21bbfae1c41c466634d958dedf580f6aa"


def build_protoc(build, archive=None):
    destination = build / "game-tools/protoc-2.6.0"
    destination.parent.mkdir(parents=True, exist_ok=True)
    if destination.is_file():
        return str(destination)
    with TemporaryDirectory(dir=destination.parent) as temp:
        work = Path(temp)
        if archive is None:
            archive = work / "release.tar.bz2"
            print("Downloading the public protobuf 2.6.0 compiler source ...", flush=True)
            with urlopen(URL, timeout=45) as response, archive.open("wb") as output:
                shutil.copyfileobj(response, output)
        if hashlib.sha256(archive.read_bytes()).hexdigest() != SHA256:
            raise ValueError("unexpected checksum for the protobuf 2.6.0 release archive")
        # Only extract the exact, checksum-pinned upstream distribution.
        with tarfile.open(archive, "r:bz2") as tar:
            if hasattr(tarfile, "data_filter"):
                tar.extractall(work, filter="data")
            else:
                tar.extractall(work)
        source = work / "protobuf-2.6.0"
        env = {**os.environ, "CXXFLAGS": "-O2 -std=c++11"}
        subprocess.run(["./configure", "--disable-shared"], cwd=source, env=env, check=True)
        subprocess.run(["make", "-C", "src", "-j4", "protoc"], cwd=source, env=env, check=True)
        binary = source / "src/protoc"
        if subprocess.check_output([binary, "--version"], text=True).strip() != "libprotoc 2.6.0":
            raise ValueError("built compiler returned an unexpected version")
        binary.replace(destination)
    return str(destination)
