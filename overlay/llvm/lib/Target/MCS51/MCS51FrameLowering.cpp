//===-- MCS51FrameLowering.cpp - MCS51 frame lowering --------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "MCS51FrameLowering.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

MCS51FrameLowering::MCS51FrameLowering()
    : TargetFrameLowering(StackGrowsDown, Align(1), 0, Align(1)) {}

void MCS51FrameLowering::emitPrologue(MachineFunction &MF,
                                      MachineBasicBlock &MBB) const {
  if (MF.getFrameInfo().getStackSize() != 0)
    report_fatal_error("MCS51 MVP backend does not support stack frames yet");
}

void MCS51FrameLowering::emitEpilogue(MachineFunction &MF,
                                      MachineBasicBlock &MBB) const {
  if (MF.getFrameInfo().getStackSize() != 0)
    report_fatal_error("MCS51 MVP backend does not support stack frames yet");
}

bool MCS51FrameLowering::hasFPImpl(const MachineFunction &MF) const {
  return false;
}
