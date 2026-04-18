from __future__ import annotations

import shutil

from scripts.bootstrap_llvm import bootstrap
from scripts.common import llvm_build_dir, run


def _read_cmake_cache_entry(cache_file, key: str) -> str | None:
    prefix = f"{key}:INTERNAL="
    for line in cache_file.read_text().splitlines():
        if line.startswith(prefix):
            return line[len(prefix) :]
    return None


def _ensure_compatible_build_dir(build_dir, src_dir) -> None:
    cache_file = build_dir / "CMakeCache.txt"
    if not cache_file.exists():
        return

    expected_build_dir = str(build_dir)
    expected_source_dir = str(src_dir / "llvm")
    cached_build_dir = _read_cmake_cache_entry(cache_file, "CMAKE_CACHEFILE_DIR")
    cached_source_dir = _read_cmake_cache_entry(cache_file, "CMAKE_HOME_DIRECTORY")

    if cached_build_dir != expected_build_dir or cached_source_dir != expected_source_dir:
        shutil.rmtree(build_dir)


def build() -> None:
    src_dir = bootstrap()
    build_dir = llvm_build_dir()
    _ensure_compatible_build_dir(build_dir, src_dir)
    build_dir.mkdir(parents=True, exist_ok=True)

    run(
        [
            "cmake",
            "-G",
            "Ninja",
            "-S",
            str(src_dir / "llvm"),
            "-B",
            str(build_dir),
            "-DLLVM_ENABLE_PROJECTS=",
            "-DLLVM_TARGETS_TO_BUILD=",
            "-DLLVM_EXPERIMENTAL_TARGETS_TO_BUILD=MCS51",
            "-DCMAKE_BUILD_TYPE=Release",
        ]
    )
    run(
        [
            "cmake",
            "--build",
            str(build_dir),
            "--target",
            "MCS51CommonTableGen",
            "LLVMMCS51Info",
            "LLVMMCS51Desc",
            "LLVMMCS51CodeGen",
            "llc",
            "FileCheck",
        ]
    )


if __name__ == "__main__":
    build()
    print(f"LLVM backend build is ready at {llvm_build_dir()}")
