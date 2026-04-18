//===-- MCS51TargetInfo.cpp - MCS51 target registration ------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "TargetInfo/MCS51TargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Compiler.h"

using namespace llvm;

Target &llvm::getTheMCS51Target() {
  static Target TheMCS51Target;
  return TheMCS51Target;
}

extern "C" LLVM_ABI LLVM_EXTERNAL_VISIBILITY void
LLVMInitializeMCS51TargetInfo() {
  RegisterTarget<> X(getTheMCS51Target(), "mcs51", "MCS-51", "MCS51");
}
