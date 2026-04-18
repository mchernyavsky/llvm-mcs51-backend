//===-- MCS51.td - Describe the MCS-51 target machine -------*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

include "llvm/Target/Target.td"

class Proc<string Name, list<SubtargetFeature> Features>
    : Processor<Name, NoItineraries, Features>;

def : Proc<"generic", []>;
def : Proc<"mcs51", []>;

include "MCS51RegisterInfo.td"
include "MCS51CallingConv.td"
include "MCS51InstrInfo.td"

def MCS51InstrInfo : InstrInfo;

def MCS51AsmWriter : AsmWriter {
  string AsmWriterClassName = "InstPrinter";
}

def MCS51 : Target {
  let InstructionSet = MCS51InstrInfo;
}
