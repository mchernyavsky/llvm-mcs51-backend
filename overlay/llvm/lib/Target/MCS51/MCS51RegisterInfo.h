//===-- MCS51RegisterInfo.h - MCS51 register info --------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_MCS51_MCS51REGISTERINFO_H
#define LLVM_LIB_TARGET_MCS51_MCS51REGISTERINFO_H

#include "MCS51.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"

#define GET_REGINFO_HEADER
#include "MCS51GenRegisterInfo.inc"

namespace llvm {

class MCS51RegisterInfo : public MCS51GenRegisterInfo {
public:
  MCS51RegisterInfo();

  const MCPhysReg *getCalleeSavedRegs(const MachineFunction *MF) const override;
  const uint32_t *getCallPreservedMask(const MachineFunction &MF,
                                       CallingConv::ID CC) const override;
  BitVector getReservedRegs(const MachineFunction &MF) const override;
  const TargetRegisterClass *
  getPointerRegClass(const MachineFunction &MF,
                     unsigned Kind = 0) const override;
  bool eliminateFrameIndex(MachineBasicBlock::iterator II, int SPAdj,
                           unsigned FIOperandNum,
                           RegScavenger *RS = nullptr) const override;
  Register getFrameRegister(const MachineFunction &MF) const override;
};

} // namespace llvm

#endif
