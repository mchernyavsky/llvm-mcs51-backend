//===-- MCS51RegisterInfo.cpp - MCS51 register info ----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "MCS51RegisterInfo.h"
#include "MCS51.h"
#include "MCS51FrameLowering.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

#define GET_REGINFO_TARGET_DESC
#include "MCS51GenRegisterInfo.inc"

MCS51RegisterInfo::MCS51RegisterInfo() : MCS51GenRegisterInfo(MCS51::PC) {}

const MCPhysReg *
MCS51RegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
  return CSR_NoRegs_SaveList;
}

const uint32_t *
MCS51RegisterInfo::getCallPreservedMask(const MachineFunction &MF,
                                        CallingConv::ID CC) const {
  return CSR_NoRegs_RegMask;
}

BitVector MCS51RegisterInfo::getReservedRegs(const MachineFunction &MF) const {
  BitVector Reserved(getNumRegs());
  Reserved.set(MCS51::A);
  Reserved.set(MCS51::B);
  Reserved.set(MCS51::PSW);
  Reserved.set(MCS51::SP);
  Reserved.set(MCS51::PC);
  return Reserved;
}

const TargetRegisterClass *
MCS51RegisterInfo::getPointerRegClass(const MachineFunction &MF,
                                      unsigned Kind) const {
  return &MCS51::GPR8RegClass;
}

bool MCS51RegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II,
                                            int SPAdj, unsigned FIOperandNum,
                                            RegScavenger *RS) const {
  report_fatal_error("MCS51 MVP backend does not support frame indices yet");
}

Register MCS51RegisterInfo::getFrameRegister(const MachineFunction &MF) const {
  return MCS51::SP;
}
