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

define i8 @shl1_u8(i8 %a) {
entry:
  %res = shl i8 %a, 1
  ret i8 %res
}

; CHECK-LABEL: shl1_u8:
; CHECK: mov a, r7
; CHECK: clr c
; CHECK: rlc a
; CHECK: mov r7, a
; CHECK: mov a, r7
; CHECK: ret

define i8 @shl3_u8(i8 %a) {
entry:
  %res = shl i8 %a, 3
  ret i8 %res
}

; CHECK-LABEL: shl3_u8:
; CHECK: mov a, r7
; CHECK: clr c
; CHECK: rlc a
; CHECK: clr c
; CHECK: rlc a
; CHECK: clr c
; CHECK: rlc a
; CHECK: mov r7, a
; CHECK: mov a, r7
; CHECK: ret

define i8 @lshr1_u8(i8 %a) {
entry:
  %res = lshr i8 %a, 1
  ret i8 %res
}

; CHECK-LABEL: lshr1_u8:
; CHECK: mov a, r7
; CHECK: clr c
; CHECK: rrc a
; CHECK: mov r7, a
; CHECK: mov a, r7
; CHECK: ret

define i8 @lshr3_u8(i8 %a) {
entry:
  %res = lshr i8 %a, 3
  ret i8 %res
}

; CHECK-LABEL: lshr3_u8:
; CHECK: mov a, r7
; CHECK: clr c
; CHECK: rrc a
; CHECK: clr c
; CHECK: rrc a
; CHECK: clr c
; CHECK: rrc a
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

define i8 @mul_u8(i8 %a, i8 %b) {
entry:
  %prod = mul i8 %a, %b
  ret i8 %prod
}

; CHECK-LABEL: mul_u8:
; CHECK: mov a, r7
; CHECK: mov b, r6
; CHECK: mul ab
; CHECK: mov r7, a
; CHECK: mov a, r7
; CHECK: ret

define i8 @mul_imm_u8(i8 %a) {
entry:
  %prod = mul i8 %a, 13
  ret i8 %prod
}

; CHECK-LABEL: mul_imm_u8:
; CHECK: mov a, r7
; CHECK: mov b, #13
; CHECK: mul ab
; CHECK: mov r7, a
; CHECK: mov a, r7
; CHECK: ret

define i8 @udiv_u8(i8 %a, i8 %b) {
entry:
  %quot = udiv i8 %a, %b
  ret i8 %quot
}

; CHECK-LABEL: udiv_u8:
; CHECK: mov a, r7
; CHECK: mov b, r6
; CHECK: div ab
; CHECK: mov r7, a
; CHECK: mov a, r7
; CHECK: ret

define i8 @urem_u8(i8 %a, i8 %b) {
entry:
  %rem = urem i8 %a, %b
  ret i8 %rem
}

; CHECK-LABEL: urem_u8:
; CHECK: mov a, r7
; CHECK: mov b, r6
; CHECK: div ab
; CHECK: mov r7, b
; CHECK: mov a, r7
; CHECK: ret

define i8 @udiv_imm_u8(i8 %a) {
entry:
  %quot = udiv i8 %a, 13
  ret i8 %quot
}

; CHECK-LABEL: udiv_imm_u8:
; CHECK: mov [[DIVREG:r[0-6]]], #13
; CHECK: mov a, r7
; CHECK: mov b, [[DIVREG]]
; CHECK: div ab
; CHECK: mov r7, a
; CHECK: mov a, r7
; CHECK: ret

define i8 @udiv_imm_highbit_u8(i8 %a) {
entry:
  %quot = udiv i8 %a, 200
  ret i8 %quot
}

; CHECK-LABEL: udiv_imm_highbit_u8:
; CHECK: mov [[HIGHDIV:r[0-6]]], #200
; CHECK: mov a, r7
; CHECK: mov b, [[HIGHDIV]]
; CHECK: div ab
; CHECK: mov r7, a
; CHECK: mov a, r7
; CHECK: ret

define i8 @urem_imm_u8(i8 %a) {
entry:
  %rem = urem i8 %a, 13
  ret i8 %rem
}

; CHECK-LABEL: urem_imm_u8:
; CHECK: mov [[REMREG:r[0-6]]], #13
; CHECK: mov a, r7
; CHECK: mov b, [[REMREG]]
; CHECK: div ab
; CHECK: mov r7, b
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
; CHECK: xrl a, #200
; CHECK: add a, #255
; CHECK: clr a
; CHECK: rlc a
; CHECK: mov r7, a
; CHECK: mov a, r7
; CHECK: ret
