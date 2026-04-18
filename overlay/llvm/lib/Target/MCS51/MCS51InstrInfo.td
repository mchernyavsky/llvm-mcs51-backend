//===-- MCS51InstrInfo.td - MCS51 instruction definitions ---*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

include "MCS51InstrFormats.td"

def MCS51retglue : SDNode<"MCS51ISD::RET_FLAG", SDTNone,
                         [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;

let isCodeGenOnly = 1, Defs = [SP], Uses = [SP] in {
  def ADJCALLSTACKDOWN
      : MCS51Pseudo<(outs), (ins i16imm:$amt1, i16imm:$amt2),
                    "#ADJCALLSTACKDOWN $amt1 $amt2">;
  def ADJCALLSTACKUP
      : MCS51Pseudo<(outs), (ins i16imm:$amt1, i16imm:$amt2),
                    "#ADJCALLSTACKUP $amt1 $amt2">;
}

def MOV8ri : MCS51Inst<"mov $dst, $src", (outs GPR8:$dst), (ins i8imm:$src),
                       [(set GPR8:$dst, imm:$src)]> {
  let Size = 2;
  let isMoveImm = 1;
  let isAsCheapAsAMove = 1;
}

def MOV8rr : MCS51Pseudo<(outs GPR8:$dst), (ins GPR8:$src), "",
                         [(set GPR8:$dst, GPR8:$src)]> {
  let isAsCheapAsAMove = 1;
  let isMoveReg = 1;
}

let isCommutable = 1 in {
  def ADD8rr : MCS51Pseudo<(outs GPR8:$dst), (ins GPR8:$lhs, GPR8:$rhs), "",
                           [(set GPR8:$dst, (add GPR8:$lhs, GPR8:$rhs))]>;
  def ADD8ri : MCS51Pseudo<(outs GPR8:$dst), (ins GPR8:$lhs, i8imm:$rhs), "",
                           [(set GPR8:$dst, (add GPR8:$lhs, imm:$rhs))]>;
  def AND8rr : MCS51Pseudo<(outs GPR8:$dst), (ins GPR8:$lhs, GPR8:$rhs), "",
                           [(set GPR8:$dst, (and GPR8:$lhs, GPR8:$rhs))]>;
  def AND8ri : MCS51Pseudo<(outs GPR8:$dst), (ins GPR8:$lhs, i8imm:$rhs), "",
                           [(set GPR8:$dst, (and GPR8:$lhs, imm:$rhs))]>;
  def OR8rr  : MCS51Pseudo<(outs GPR8:$dst), (ins GPR8:$lhs, GPR8:$rhs), "",
                           [(set GPR8:$dst, (or GPR8:$lhs, GPR8:$rhs))]>;
  def OR8ri  : MCS51Pseudo<(outs GPR8:$dst), (ins GPR8:$lhs, i8imm:$rhs), "",
                           [(set GPR8:$dst, (or GPR8:$lhs, imm:$rhs))]>;
  def XOR8rr : MCS51Pseudo<(outs GPR8:$dst), (ins GPR8:$lhs, GPR8:$rhs), "",
                           [(set GPR8:$dst, (xor GPR8:$lhs, GPR8:$rhs))]>;
  def XOR8ri : MCS51Pseudo<(outs GPR8:$dst), (ins GPR8:$lhs, i8imm:$rhs), "",
                           [(set GPR8:$dst, (xor GPR8:$lhs, imm:$rhs))]>;
  def MUL8rr : MCS51Pseudo<(outs GPR8:$dst), (ins GPR8:$lhs, GPR8:$rhs), "",
                           [(set GPR8:$dst, (mul GPR8:$lhs, GPR8:$rhs))]>;
  def MUL8ri : MCS51Pseudo<(outs GPR8:$dst), (ins GPR8:$lhs, i8imm:$rhs), "",
                           [(set GPR8:$dst, (mul GPR8:$lhs, imm:$rhs))]>;
}

def SUB8rr : MCS51Pseudo<(outs GPR8:$dst), (ins GPR8:$lhs, GPR8:$rhs), "",
                         [(set GPR8:$dst, (sub GPR8:$lhs, GPR8:$rhs))]>;
def SUB8ri : MCS51Pseudo<(outs GPR8:$dst), (ins GPR8:$lhs, i8imm:$rhs), "",
                         [(set GPR8:$dst, (sub GPR8:$lhs, imm:$rhs))]>;

def CMP8rr : MCS51Pseudo<(outs GPR8:$dst),
                         (ins GPR8:$lhs, GPR8:$rhs, i8imm:$flags)>;
def CMP8ri : MCS51Pseudo<(outs GPR8:$dst),
                         (ins GPR8:$lhs, i8imm:$rhs, i8imm:$flags)>;
def CMP8ir : MCS51Pseudo<(outs GPR8:$dst),
                         (ins i8imm:$lhs, GPR8:$rhs, i8imm:$flags)>;
def CMP1rr : MCS51Pseudo<(outs GPR1:$dst),
                         (ins GPR8:$lhs, GPR8:$rhs, i8imm:$flags)>;
def CMP1ri : MCS51Pseudo<(outs GPR1:$dst),
                         (ins GPR8:$lhs, i8imm:$rhs, i8imm:$flags)>;
def CMP1ir : MCS51Pseudo<(outs GPR1:$dst),
                         (ins i8imm:$lhs, GPR8:$rhs, i8imm:$flags)>;

let Defs = [A], isAsCheapAsAMove = 1 in {
  def MOVA_r : MCS51Inst<"mov a, $src", (outs), (ins GPR8:$src)>;
  def MOVA_i : MCS51Inst<"mov a, $src", (outs), (ins i8imm:$src)> {
    let Size = 2;
  }
  def CLR_A  : MCS51Inst<"clr a", (outs), (ins)>;
}

let Defs = [B], isAsCheapAsAMove = 1 in {
  def MOVB_r : MCS51Inst<"mov b, $src", (outs), (ins GPR8:$src)> {
    let Size = 2;
  }
  def MOVB_i : MCS51Inst<"mov b, $src", (outs), (ins i8imm:$src)> {
    let Size = 3;
  }
}

let Defs = [A, PSW], Uses = [A, PSW] in
  def RLC_A : MCS51Inst<"rlc a", (outs), (ins)>;

let Defs = [A, B, PSW], Uses = [A, B] in
  def MUL_AB : MCS51Inst<"mul ab", (outs), (ins)>;

let Uses = [A], isAsCheapAsAMove = 1 in
  def MOVR_a : MCS51Inst<"mov $dst, a", (outs GPR8:$dst), (ins)>;

let Defs = [A], Uses = [A] in {
  def ADDA_r : MCS51Inst<"add a, $src", (outs), (ins GPR8:$src)>;
  def ADDA_i : MCS51Inst<"add a, $src", (outs), (ins i8imm:$src)> {
    let Size = 2;
  }
  def ANLA_r : MCS51Inst<"anl a, $src", (outs), (ins GPR8:$src)>;
  def ANLA_i : MCS51Inst<"anl a, $src", (outs), (ins i8imm:$src)> {
    let Size = 2;
  }
  def ORLA_r : MCS51Inst<"orl a, $src", (outs), (ins GPR8:$src)>;
  def ORLA_i : MCS51Inst<"orl a, $src", (outs), (ins i8imm:$src)> {
    let Size = 2;
  }
  def XRLA_r : MCS51Inst<"xrl a, $src", (outs), (ins GPR8:$src)>;
  def XRLA_i : MCS51Inst<"xrl a, $src", (outs), (ins i8imm:$src)> {
    let Size = 2;
  }
}

let Defs = [PSW] in
  def CLR_C : MCS51Inst<"clr c", (outs), (ins)>;

let Defs = [A, PSW], Uses = [A, PSW] in {
  def SUBBA_r : MCS51Inst<"subb a, $src", (outs), (ins GPR8:$src)>;
  def SUBBA_i : MCS51Inst<"subb a, $src", (outs), (ins i8imm:$src)> {
    let Size = 2;
  }
}

let isReturn = 1, isTerminator = 1, isBarrier = 1, Uses = [A] in
  def RET : MCS51Inst<"ret", (outs), (ins), [(MCS51retglue)]>;
