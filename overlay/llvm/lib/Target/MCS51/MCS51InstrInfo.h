//===-- MCS51InstrInfo.h - MCS51 instruction info --------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_MCS51_MCS51INSTRINFO_H
#define LLVM_LIB_TARGET_MCS51_MCS51INSTRINFO_H

#include "MCS51.h"
#include "MCS51RegisterInfo.h"
#include "llvm/CodeGen/TargetInstrInfo.h"

#define GET_INSTRINFO_HEADER
#include "MCS51GenInstrInfo.inc"

namespace llvm {

class MCS51Subtarget;

class MCS51InstrInfo : public MCS51GenInstrInfo {
  MCS51RegisterInfo RI;

public:
  explicit MCS51InstrInfo(const MCS51Subtarget &STI);

  const MCS51RegisterInfo &getRegisterInfo() const { return RI; }

  void copyPhysReg(MachineBasicBlock &MBB, MachineBasicBlock::iterator I,
                   const DebugLoc &DL, Register DestReg, Register SrcReg,
                   bool KillSrc, bool RenamableDest,
                   bool RenamableSrc) const override;

  void storeRegToStackSlot(MachineBasicBlock &MBB,
                           MachineBasicBlock::iterator I, Register SrcReg,
                           bool IsKill, int FrameIndex,
                           const TargetRegisterClass *RC,
                           const TargetRegisterInfo *TRI, Register VReg,
                           MachineInstr::MIFlag Flags) const override;

  void loadRegFromStackSlot(MachineBasicBlock &MBB,
                            MachineBasicBlock::iterator I, Register DestReg,
                            int FrameIndex, const TargetRegisterClass *RC,
                            const TargetRegisterInfo *TRI, Register VReg,
                            MachineInstr::MIFlag Flags) const override;
};

} // namespace llvm

#endif
