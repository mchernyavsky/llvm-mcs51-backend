; RUN: llc -march=mcs51 -verify-machineinstrs -filetype=asm < %s | FileCheck %s

define i8 @const_42() {
entry:
  ret i8 42
}

; CHECK-LABEL: const_42:
; CHECK: mov r7, #42
; CHECK: mov a, r7
; CHECK: ret

define i8 @add_u8(i8 %a, i8 %b) {
entry:
  %sum = add i8 %a, %b
  ret i8 %sum
}

; CHECK-LABEL: add_u8:
; CHECK: mov a, r7
; CHECK: add a, r6
; CHECK: mov r7, a
; CHECK: mov a, r7
; CHECK: ret

define i8 @sub_u8(i8 %a, i8 %b) {
entry:
  %diff = sub i8 %a, %b
  ret i8 %diff
}

; CHECK-LABEL: sub_u8:
; CHECK: mov a, r7
; CHECK: clr c
; CHECK: subb a, r6
; CHECK: mov r7, a
; CHECK: mov a, r7
; CHECK: ret

define i8 @xor_imm(i8 %a) {
entry:
  %x = xor i8 %a, 90
  ret i8 %x
}

; CHECK-LABEL: xor_imm:
; CHECK: mov a, r7
; CHECK: xrl a, #90
; CHECK: mov r7, a
; CHECK: mov a, r7
; CHECK: ret

define i8 @ult_u8(i8 %a, i8 %b) {
entry:
  %cmp = icmp ult i8 %a, %b
  %res = zext i1 %cmp to i8
  ret i8 %res
}

; CHECK-LABEL: ult_u8:
; CHECK: mov a, r7
; CHECK: clr c
; CHECK: subb a, r6
; CHECK: clr a
; CHECK: rlc a
; CHECK: mov r7, a
; CHECK: mov a, r7
; CHECK: ret

define i8 @ugt_u8(i8 %a, i8 %b) {
entry:
  %cmp = icmp ugt i8 %a, %b
  %res = zext i1 %cmp to i8
  ret i8 %res
}

; CHECK-LABEL: ugt_u8:
; CHECK: mov a, r6
; CHECK: clr c
; CHECK: subb a, r7
; CHECK: clr a
; CHECK: rlc a
; CHECK: mov r7, a
; CHECK: mov a, r7
; CHECK: ret

define i8 @uge_imm_u8(i8 %a) {
entry:
  %cmp = icmp uge i8 %a, 5
  %res = zext i1 %cmp to i8
  ret i8 %res
}

; CHECK-LABEL: uge_imm_u8:
; CHECK: mov a, #4
; CHECK: clr c
; CHECK: subb a, r7
; CHECK: clr a
; CHECK: rlc a
; CHECK: mov r7, a
; CHECK: mov a, r7
; CHECK: ret

define i8 @ule_imm_u8(i8 %a) {
entry:
  %cmp = icmp ule i8 %a, 5
  %res = zext i1 %cmp to i8
  ret i8 %res
}

; CHECK-LABEL: ule_imm_u8:
; CHECK: mov a, r7
; CHECK: clr c
; CHECK: subb a, #6
; CHECK: clr a
; CHECK: rlc a
; CHECK: mov r7, a
; CHECK: mov a, r7
; CHECK: ret

define i8 @uge_u8(i8 %a, i8 %b) {
entry:
  %cmp = icmp uge i8 %a, %b
  %res = zext i1 %cmp to i8
  ret i8 %res
}

; CHECK-LABEL: uge_u8:
; CHECK: mov a, r7
; CHECK: clr c
; CHECK: subb a, r6
; CHECK: clr a
; CHECK: rlc a
; CHECK: xrl a, #1
; CHECK: mov r7, a
; CHECK: mov a, r7
; CHECK: ret

define i8 @ule_u8(i8 %a, i8 %b) {
entry:
  %cmp = icmp ule i8 %a, %b
  %res = zext i1 %cmp to i8
  ret i8 %res
}

; CHECK-LABEL: ule_u8:
; CHECK: mov a, r6
; CHECK: clr c
; CHECK: subb a, r7
; CHECK: clr a
; CHECK: rlc a
; CHECK: xrl a, #1
; CHECK: mov r7, a
; CHECK: mov a, r7
; CHECK: ret

define i8 @eq_u8(i8 %a, i8 %b) {
entry:
  %cmp = icmp eq i8 %a, %b
  %res = zext i1 %cmp to i8
  ret i8 %res
}

; CHECK-LABEL: eq_u8:
; CHECK: mov a, r7
; CHECK: xrl a, r6
; CHECK: add a, #255
; CHECK: clr a
; CHECK: rlc a
; CHECK: xrl a, #1
; CHECK: mov r7, a
; CHECK: mov a, r7
; CHECK: ret

define i8 @ne_imm_u8(i8 %a) {
entry:
  %cmp = icmp ne i8 %a, 200
  %res = zext i1 %cmp to i8
  ret i8 %res
}

; CHECK-LABEL: ne_imm_u8:
; CHECK: mov a, r7
; CHECK: xrl a, #-56
; CHECK: add a, #255
; CHECK: clr a
; CHECK: rlc a
; CHECK: mov r7, a
; CHECK: mov a, r7
; CHECK: ret
