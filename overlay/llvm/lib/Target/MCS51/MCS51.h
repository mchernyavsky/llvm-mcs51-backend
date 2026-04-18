//===-- MCS51.h - Top-level interface for MCS51 backend --------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_MCS51_MCS51_H
#define LLVM_LIB_TARGET_MCS51_MCS51_H

#include "MCTargetDesc/MCS51MCTargetDesc.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/SelectionDAGNodes.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {

class MCS51TargetMachine;
class PassRegistry;

FunctionPass *createMCS51ISelDag(MCS51TargetMachine &TM, CodeGenOptLevel OL);
void initializeMCS51AsmPrinterPass(PassRegistry &);
void initializeMCS51DAGToDAGISelLegacyPass(PassRegistry &);

namespace MCS51ISD {
enum NodeType : unsigned {
  FIRST_NUMBER = ISD::BUILTIN_OP_END,
  RET_FLAG
};
} // namespace MCS51ISD

} // namespace llvm

#endif
