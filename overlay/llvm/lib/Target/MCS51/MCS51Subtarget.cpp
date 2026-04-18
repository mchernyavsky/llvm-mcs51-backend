//===-- MCS51Subtarget.cpp - MCS51 subtarget information -----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "MCS51Subtarget.h"
#include "llvm/Support/Debug.h"

using namespace llvm;

#define DEBUG_TYPE "mcs51-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "MCS51GenSubtargetInfo.inc"

MCS51Subtarget &
MCS51Subtarget::initializeSubtargetDependencies(StringRef CPU, StringRef FS) {
  StringRef CPUName = CPU.empty() ? StringRef("generic") : CPU;
  ParseSubtargetFeatures(CPUName, CPUName, FS);
  return *this;
}

MCS51Subtarget::MCS51Subtarget(const Triple &TT, StringRef CPU, StringRef FS,
                               const TargetMachine &TM)
    : MCS51GenSubtargetInfo(TT, CPU, CPU, FS),
      InstrInfo(initializeSubtargetDependencies(CPU, FS)), TLInfo(TM, *this),
      TSInfo(), FrameLowering() {}
