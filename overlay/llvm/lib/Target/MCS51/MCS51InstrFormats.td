//===-- MCS51InstrFormats.td - MCS51 instruction formats ----*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

class MCS51Inst<string Asm, dag Outs, dag Ins, list<dag> Pat = []>
    : Instruction {
  let Namespace = "MCS51";
  let OutOperandList = Outs;
  let InOperandList = Ins;
  let AsmString = Asm;
  let Pattern = Pat;
  let Size = 1;
}

class MCS51Pseudo<dag Outs, dag Ins, string Asm = "", list<dag> Pat = []>
    : MCS51Inst<Asm, Outs, Ins, Pat> {
  let isPseudo = 1;
  let isCodeGenOnly = 1;
}
