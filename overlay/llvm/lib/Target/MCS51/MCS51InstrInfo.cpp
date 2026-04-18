//===-- MCS51InstrInfo.cpp - MCS51 instruction info ----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "MCS51InstrInfo.h"
#include "MCS51.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

#define GET_INSTRINFO_CTOR_DTOR
#include "MCS51GenInstrInfo.inc"

MCS51InstrInfo::MCS51InstrInfo(const MCS51Subtarget &STI)
    : MCS51GenInstrInfo(MCS51::ADJCALLSTACKDOWN, MCS51::ADJCALLSTACKUP),
      RI() {}

void MCS51InstrInfo::copyPhysReg(MachineBasicBlock &MBB,
                                 MachineBasicBlock::iterator I,
                                 const DebugLoc &DL, Register DestReg,
                                 Register SrcReg, bool KillSrc,
                                 bool RenamableDest,
                                 bool RenamableSrc) const {
  if (DestReg == SrcReg)
    return;

  if (MCS51::GPR8RegClass.contains(DestReg) &&
      MCS51::GPR8RegClass.contains(SrcReg)) {
    BuildMI(MBB, I, DL, get(MCS51::MOV8rr), DestReg)
        .addReg(SrcReg, getKillRegState(KillSrc));
    return;
  }

  if (DestReg == MCS51::A && MCS51::GPR8RegClass.contains(SrcReg)) {
    BuildMI(MBB, I, DL, get(MCS51::MOVA_r))
        .addReg(SrcReg, getKillRegState(KillSrc));
    return;
  }

  if (MCS51::GPR8RegClass.contains(DestReg) && SrcReg == MCS51::A) {
    BuildMI(MBB, I, DL, get(MCS51::MOVR_a), DestReg);
    return;
  }

  report_fatal_error("Unsupported MCS51 physical register copy");
}

void MCS51InstrInfo::storeRegToStackSlot(
    MachineBasicBlock &MBB, MachineBasicBlock::iterator I, Register SrcReg,
    bool IsKill, int FrameIndex, const TargetRegisterClass *RC,
    const TargetRegisterInfo *TRI, Register VReg,
    MachineInstr::MIFlag Flags) const {
  report_fatal_error("MCS51 MVP backend does not support stack spills yet");
}

void MCS51InstrInfo::loadRegFromStackSlot(
    MachineBasicBlock &MBB, MachineBasicBlock::iterator I, Register DestReg,
    int FrameIndex, const TargetRegisterClass *RC, const TargetRegisterInfo *TRI,
    Register VReg, MachineInstr::MIFlag Flags) const {
  report_fatal_error("MCS51 MVP backend does not support stack reloads yet");
}
