//===-- MCS51FrameLowering.h - MCS51 frame lowering ------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_MCS51_MCS51FRAMELOWERING_H
#define LLVM_LIB_TARGET_MCS51_MCS51FRAMELOWERING_H

#include "MCS51.h"
#include "llvm/CodeGen/TargetFrameLowering.h"

namespace llvm {

class MCS51FrameLowering : public TargetFrameLowering {
protected:
  bool hasFPImpl(const MachineFunction &MF) const override;

public:
  MCS51FrameLowering();

  void emitPrologue(MachineFunction &MF, MachineBasicBlock &MBB) const override;
  void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const override;
};

} // namespace llvm

#endif
