from __future__ import annotations

import json
import re
from dataclasses import dataclass

from scripts.bootstrap_llvm import bootstrap
from scripts.build_llvm import build
from scripts.common import ensure_any_tool, ensure_tool, llvm_build_dir, out_root, repo_root, run
from tools.mcs51_flat_image import build_flat_image, write_intel_hex


P1_RE = re.compile(r"0x90\s+([0-9A-Fa-f]{2})\b")


@dataclass
class E2ECase:
    name: str
    file: str
    function: str
    args: list[int]
    expected: int


def load_e2e_cases() -> list[E2ECase]:
    raw = json.loads((repo_root() / "tests" / "e2e" / "cases.json").read_text())
    return [E2ECase(**item) for item in raw]


def parse_p1(output: str) -> int:
    matches = P1_RE.findall(output)
    if not matches:
        raise SystemExit("Could not parse P1 value from emulator output")
    return int(matches[-1], 16)


def run_codegen_tests() -> None:
    src_dir = bootstrap()
    build_dir = llvm_build_dir()
    llc = build_dir / "bin" / "llc"
    filecheck = build_dir / "bin" / "FileCheck"
    test_file = src_dir / "llvm" / "test" / "CodeGen" / "MCS51" / "basic-i8.ll"
    asm_file = build_dir / "basic-i8.s"
    obj_file = build_dir / "basic-i8.o"

    if not llc.exists():
        raise SystemExit(f"Missing llc binary: {llc}")
    if not filecheck.exists():
        raise SystemExit(f"Missing FileCheck binary: {filecheck}")

    run(
        [
            str(llc),
            "-march=mcs51",
            "-mtriple=mcs51-unknown-elf",
            "-verify-machineinstrs",
            "-filetype=asm",
            "-o",
            str(asm_file),
            str(test_file),
        ]
    )
    run([str(filecheck), str(test_file)], input_text=asm_file.read_text())
    run(
        [
            str(llc),
            "-march=mcs51",
            "-mtriple=mcs51-unknown-elf",
            "-verify-machineinstrs",
            "-filetype=obj",
            "-o",
            str(obj_file),
            str(test_file),
        ]
    )
    if obj_file.stat().st_size == 0:
        raise SystemExit(f"Empty object file produced by llc: {obj_file}")


def run_e2e_case(case: E2ECase) -> None:
    build_dir = llvm_build_dir()
    llc = build_dir / "bin" / "llc"
    if not llc.exists():
        raise SystemExit(f"Missing llc binary: {llc}. Run build first.")

    clang = ensure_tool("clang")
    ucsim = ensure_any_tool("ucsim_51", "s51")

    case_root = out_root() / "e2e" / case.name
    case_root.mkdir(parents=True, exist_ok=True)

    source = repo_root() / "tests" / "e2e" / case.file
    llvm_ir = case_root / f"{case.name}.ll"
    llvm_obj = case_root / f"{case.name}.o"
    image_ihx = case_root / f"{case.name}.ihx"
    image_bin = case_root / f"{case.name}.bin"

    run([clang, "-O2", "-S", "-emit-llvm", str(source), "-o", str(llvm_ir)])
    run(
        [
            str(llc),
            "-march=mcs51",
            "-mtriple=mcs51-unknown-elf",
            "-verify-machineinstrs",
            "-filetype=obj",
            "-o",
            str(llvm_obj),
            str(llvm_ir),
        ]
    )

    flat_image = build_flat_image(llvm_obj, case.function, case.args)
    image_bin.write_bytes(flat_image.image)
    write_intel_hex(image_ihx, flat_image.image)

    breakpoint = flat_image.done_address
    commands = f"break 0x{breakpoint:x}\nrun\nds 0x90 0x90\nquit\n"
    emulator = run([ucsim, "-t", "51", "-c", "-", str(image_ihx)], input_text=commands)
    actual = parse_p1(emulator.stdout)

    if actual != case.expected:
        raise SystemExit(
            f"{case.name}: emulator result mismatch, expected 0x{case.expected:02x}, got 0x{actual:02x}"
        )

    print(
        f"{case.name}: OK "
        f"(expected=0x{case.expected:02x}, got=0x{actual:02x}, obj={llvm_obj}, ihx={image_ihx}, bin={image_bin})"
    )


def run_e2e_tests() -> None:
    bootstrap()
    for case in load_e2e_cases():
        run_e2e_case(case)


def main() -> None:
    build()
    run_codegen_tests()
    run_e2e_tests()
    print("All LLVM codegen and end-to-end tests passed.")


if __name__ == "__main__":
    main()
