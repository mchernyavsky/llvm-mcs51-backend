; RUN: not --crash llc -march=mcs51 -mtriple=mcs51-unknown-elf -verify-machineinstrs -filetype=asm -o /dev/null < %s 2>&1 | FileCheck %s

define i8 @shl_var_u8(i8 %a, i8 %amt) {
entry:
  %res = shl i8 %a, %amt
  ret i8 %res
}

; CHECK: LLVM ERROR: MCS51 MVP backend supports only constant-count i8 left shifts
