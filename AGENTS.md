# AGENTS.md

This repository is an experimental LLVM backend for the Intel MCS-51 / 8051 family.

## Repo Layout

- `overlay/llvm/lib/Target/MCS51` — backend implementation overlay copied into upstream LLVM during bootstrap
- `overlay/llvm/test/CodeGen/MCS51/basic-i8.ll` — LLVM codegen regression coverage
- `tests/e2e` — end-to-end C test cases plus `cases.json`
- `scripts` — bootstrap/build/test entrypoints
- `tools` — runtime/image helpers used by the end-to-end harness

## Testing

There are two supported ways to validate the project:

1. Local toolchain install
   - `make bootstrap`
   - `make build`
   - `make test`
2. CI
   - GitHub Actions runs the same `make test` path via `.github/workflows/ci.yml`

When adding backend functionality, prefer to add both:

- LLVM codegen regression coverage in `overlay/llvm/test/CodeGen/MCS51/basic-i8.ll`
- end-to-end emulator coverage in `tests/e2e`

## Change Checklist

When a feature changes the supported subset, update all relevant surfaces together:

- backend code in `overlay/llvm/lib/Target/MCS51`
- codegen regression tests
- end-to-end tests
- `README.md` scope/limitations if user-visible support changed
- `AGENTS.md` if recurring repo workflow guidance or contributor expectations changed

Keep `AGENTS.md` and `README.md` current, but keep them concise. Add durable, high-signal guidance only; do not accumulate low-value reminders or transient worklog detail.

## Workflow Expectations

- Default workflow is one feature/fix per branch and one PR per branch.
- Keep PRs narrow and traceable.
- Do not merge without green CI.
- If you encounter a real bug, tooling gap, or validation blocker while working, create a GitHub issue for it so the problem is tracked even if the current branch moves on.
- If a PR implements a tracked GitHub issue, include an explicit closing keyword in the PR body such as `Closes #N`, `Fixes #N`, or `Resolves #N`.
- After requesting Copilot review, do not merge only on the basis of green CI.
- Copilot comments can arrive after checks pass, and in some cases even after a merge becomes available.
- Before merging, confirm that the Copilot review was actually submitted and inspect thread-level review comments, not just the review summary.
- If Copilot was requested, wait for the review to land or explicitly time out after a reasonable polling window before merging.
- Treat unresolved review threads as merge blockers until they are fixed or tracked in GitHub issues and explicitly resolved.
- Before merging a PR that requested Copilot review, run `python3 -m scripts.check_review_threads --repo <owner/name> --pr <number> --require-copilot` and require it to pass.
- If a review comment points out a real functional gap, either fix it or track it in GitHub issues.
- If you run target-side checks directly against `out/llvm-build`, sync `overlay` into `out/llvm-src` first via `python3 -m scripts.bootstrap_llvm`.

## Current Roadmap

- Umbrella roadmap: issue `#21`
- Near-term correctness follow-ups: issues `#7` and `#8`

## Current Backend Scope

As of this file, the backend mainly supports a small `i8` subset:

- leaf-style functions over `i8`
- a narrow register-based calling subset
- arithmetic/logical ops already listed in `README.md`
- compare materialization to `0/1`
- object emission plus emulator-based end-to-end verification

Anything beyond that should be treated as opt-in work and documented carefully.
