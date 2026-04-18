from __future__ import annotations

import shutil
from pathlib import Path

from scripts.common import (
    LLVM_REMOTE,
    LLVM_TAG,
    llvm_src_dir,
    out_root,
    overlay_root,
    repo_root,
    run,
)


def copy_overlay_file(src: Path, dst: Path) -> None:
    dst.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(src, dst)


def copy_overlay_tree(src: Path, dst: Path) -> None:
    if dst.exists():
        shutil.rmtree(dst)
    shutil.copytree(src, dst)


def bootstrap() -> Path:
    out_root().mkdir(parents=True, exist_ok=True)
    src_dir = llvm_src_dir()

    if not src_dir.exists():
        run(
            [
                "git",
                "clone",
                "--depth",
                "1",
                "--branch",
                LLVM_TAG,
                LLVM_REMOTE,
                str(src_dir),
            ],
            cwd=repo_root(),
        )

    backend_src = overlay_root() / "llvm" / "lib" / "Target" / "MCS51"
    backend_dst = src_dir / "llvm" / "lib" / "Target" / "MCS51"
    copy_overlay_tree(backend_src, backend_dst)

    codegen_test_src = overlay_root() / "llvm" / "test" / "CodeGen" / "MCS51"
    codegen_test_dst = src_dir / "llvm" / "test" / "CodeGen" / "MCS51"
    copy_overlay_tree(codegen_test_src, codegen_test_dst)

    return src_dir


if __name__ == "__main__":
    src_dir = bootstrap()
    print(f"LLVM source ready at {src_dir}")
