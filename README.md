# LLVM MCS-51 Backend

An experimental LLVM backend for the Intel MCS-51 / 8051 family.

This repository is a small standalone project built around three pieces:

- a source overlay for the `MCS51` target on top of upstream LLVM;
- host-side tools for turning LLVM-produced object files into runnable test images;
- end-to-end tests that compile small C programs, run them in an 8051 emulator, and verify the result.

The runtime pipeline tests LLVM output directly. The backend emits `MCS-51` machine code into an ELF object with `llc -filetype=obj`, and the test harness builds a flat ROM image from that object without rewriting LLVM assembly.

## Project Layout

- `overlay/llvm/lib/Target/MCS51` — target implementation overlay copied into an upstream LLVM checkout during bootstrap.
- `overlay/llvm/test/CodeGen/MCS51/basic-i8.ll` — LLVM code generation regression test for the target.
- `scripts` — host-side entrypoints for bootstrapping LLVM, building the backend, and running the full test suite.
- `tools` — small utilities used by the end-to-end harness.
- `tests/e2e` — C-based end-to-end test cases and their expected results.
- `.github/workflows/ci.yml` — GitHub Actions workflow that installs dependencies on Ubuntu and runs the full test suite.

## How It Works

1. `clang` compiles a small C test case to LLVM IR.
2. `llc -march=mcs51 -filetype=obj` produces an `MCS-51` ELF object.
3. A host-side tool extracts `.text` and the selected function symbol from that object and builds a flat ROM image in Intel HEX format.
4. `ucsim_51` executes the image and the harness checks the final value written to port `P1`.

## Quick Start

Local execution:

```bash
make bootstrap
make build
make test
```

Build artifacts and generated test images are written to `out/`.

CI runs the same `make test` entrypoint on `ubuntu-24.04`, restoring both `ccache` and the LLVM build directory to speed up repeated LLVM rebuilds.

## Tooling

Local runs require:

- `git`
- `python3`
- `pyelftools`
- `cmake`
- `ninja`
- `clang`
- `sdcc-ucsim` or another ucSim package that provides an 8051 simulator binary such as `ucsim_51` or `s51`

Low-level entrypoints are also available if you want to run individual stages:

```bash
python3 -m scripts.bootstrap_llvm
python3 -m scripts.build_llvm
python3 -m scripts.test
```

## Upstream Base

- upstream LLVM tag: `llvmorg-21.1.8`
- target integration model: experimental LLVM target named `MCS51`

## Current Scope

The backend currently supports a narrow but working subset:

- leaf functions operating on `i8`
- up to two arguments passed in `R7` and `R6`
- `i8` return values in `A`
- `icmp eq/ne` and unsigned ordering comparisons lowered to `0/1` results
- `add`, `sub`, constant-count `shl`, `mul` (low byte), `udiv`, `urem`, `and`, `or`, `xor`, and integer constants
- `llc -filetype=obj` for the supported subset
- end-to-end verification from `C` source to machine code executed in `ucsim_51`

## Limitations

- no stack frame support, spills, calls, branches, signed compares, or general C ABI yet
- the end-to-end harness currently accepts only relocation-free leaf functions from the supported subset
- the Python image packer is a small test utility, not a general-purpose 8051 linker
