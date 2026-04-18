//===-- MCS51ISelLowering.h - MCS51 DAG lowering --------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_MCS51_MCS51ISELLOWERING_H
#define LLVM_LIB_TARGET_MCS51_MCS51ISELLOWERING_H

#include "llvm/CodeGen/TargetLowering.h"

namespace llvm {

class MCS51Subtarget;

class MCS51TargetLowering : public TargetLowering {
public:
  MCS51TargetLowering(const TargetMachine &TM, const MCS51Subtarget &STI);

  EVT getSetCCResultType(const DataLayout &DL, LLVMContext &Context,
                         EVT VT) const override;

  SDValue LowerOperation(SDValue Op, SelectionDAG &DAG) const override;

  bool CanLowerReturn(CallingConv::ID CallConv, MachineFunction &MF,
                      bool IsVarArg,
                      const SmallVectorImpl<ISD::OutputArg> &Outs,
                      LLVMContext &Context, const Type *RetTy) const override;

  SDValue LowerFormalArguments(SDValue Chain, CallingConv::ID CallConv,
                               bool IsVarArg,
                               const SmallVectorImpl<ISD::InputArg> &Ins,
                               const SDLoc &DL, SelectionDAG &DAG,
                               SmallVectorImpl<SDValue> &InVals) const override;

  SDValue LowerReturn(SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
                      const SmallVectorImpl<ISD::OutputArg> &Outs,
                      const SmallVectorImpl<SDValue> &OutVals,
                      const SDLoc &DL, SelectionDAG &DAG) const override;

private:
  SDValue LowerShiftLeft(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerLogicalShiftRight(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerSetCC(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerSelect(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerSelectCC(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerZeroExtend(SDValue Op, SelectionDAG &DAG) const;
};

} // namespace llvm

#endif
