//===-- MCS51Subtarget.h - MCS51 subtarget information ---------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_MCS51_MCS51SUBTARGET_H
#define LLVM_LIB_TARGET_MCS51_MCS51SUBTARGET_H

#include "MCS51FrameLowering.h"
#include "MCS51ISelLowering.h"
#include "MCS51InstrInfo.h"
#include "llvm/CodeGen/SelectionDAGTargetInfo.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"

#define GET_SUBTARGETINFO_HEADER
#include "MCS51GenSubtargetInfo.inc"

namespace llvm {

class MCS51Subtarget : public MCS51GenSubtargetInfo {
  MCS51InstrInfo InstrInfo;
  MCS51TargetLowering TLInfo;
  SelectionDAGTargetInfo TSInfo;
  MCS51FrameLowering FrameLowering;

  MCS51Subtarget &initializeSubtargetDependencies(StringRef CPU, StringRef FS);

public:
  MCS51Subtarget(const Triple &TT, StringRef CPU, StringRef FS,
                 const TargetMachine &TM);

  const MCS51InstrInfo *getInstrInfo() const override { return &InstrInfo; }
  const MCS51RegisterInfo *getRegisterInfo() const override {
    return &InstrInfo.getRegisterInfo();
  }
  const TargetFrameLowering *getFrameLowering() const override {
    return &FrameLowering;
  }
  const MCS51TargetLowering *getTargetLowering() const override {
    return &TLInfo;
  }
  const SelectionDAGTargetInfo *getSelectionDAGInfo() const override {
    return &TSInfo;
  }

  void ParseSubtargetFeatures(StringRef CPU, StringRef TuneCPU, StringRef FS);
};

} // namespace llvm

#endif
