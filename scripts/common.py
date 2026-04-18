from __future__ import annotations

import shutil
import subprocess
from pathlib import Path


LLVM_TAG = "llvmorg-21.1.8"
LLVM_REMOTE = "https://github.com/llvm/llvm-project.git"


def repo_root() -> Path:
    return Path(__file__).resolve().parents[1]


def overlay_root() -> Path:
    return repo_root() / "overlay"


def out_root() -> Path:
    return repo_root() / "out"


def llvm_src_dir() -> Path:
    return out_root() / "llvm-src"


def llvm_build_dir() -> Path:
    return out_root() / "llvm-build"


def run(cmd: list[str], *, cwd: Path | None = None, input_text: str | None = None) -> subprocess.CompletedProcess[str]:
    print("+", " ".join(str(part) for part in cmd))
    capture_output = input_text is not None
    return subprocess.run(
        cmd,
        cwd=cwd,
        input=input_text,
        text=True,
        check=True,
        capture_output=capture_output,
    )


def ensure_tool(name: str) -> str:
    path = shutil.which(name)
    if path is None:
        raise SystemExit(f"Required tool not found in PATH: {name}")
    return path
