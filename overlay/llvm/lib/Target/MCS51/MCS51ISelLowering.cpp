//===-- MCS51ISelLowering.cpp - MCS51 DAG lowering -----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "MCS51ISelLowering.h"
#include "MCS51.h"
#include "MCS51Subtarget.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

#include "MCS51GenCallingConv.inc"

MCS51TargetLowering::MCS51TargetLowering(const TargetMachine &TM,
                                         const MCS51Subtarget &STI)
    : TargetLowering(TM) {
  addRegisterClass(MVT::i8, &MCS51::GPR8RegClass);

  computeRegisterProperties(STI.getRegisterInfo());
  setStackPointerRegisterToSaveRestore(MCS51::SP);
  setBooleanContents(ZeroOrOneBooleanContent);

  setOperationAction(ISD::ADD, MVT::i8, Legal);
  setOperationAction(ISD::SUB, MVT::i8, Legal);
  setOperationAction(ISD::AND, MVT::i8, Legal);
  setOperationAction(ISD::OR, MVT::i8, Legal);
  setOperationAction(ISD::XOR, MVT::i8, Legal);
}

bool MCS51TargetLowering::CanLowerReturn(
    CallingConv::ID CallConv, MachineFunction &MF, bool IsVarArg,
    const SmallVectorImpl<ISD::OutputArg> &Outs, LLVMContext &Context,
    const Type *RetTy) const {
  SmallVector<CCValAssign, 4> RVLocs;
  CCState CCInfo(CallConv, IsVarArg, MF, RVLocs, Context);
  return CCInfo.CheckReturn(Outs, RetCC_MCS51);
}

SDValue MCS51TargetLowering::LowerFormalArguments(
    SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
    const SmallVectorImpl<ISD::InputArg> &Ins, const SDLoc &DL,
    SelectionDAG &DAG, SmallVectorImpl<SDValue> &InVals) const {
  if (IsVarArg)
    report_fatal_error("MCS51 MVP backend does not support varargs");

  MachineFunction &MF = DAG.getMachineFunction();

  SmallVector<CCValAssign, 8> ArgLocs;
  CCState CCInfo(CallConv, IsVarArg, MF, ArgLocs, *DAG.getContext());
  CCInfo.AnalyzeFormalArguments(Ins, CC_MCS51);

  for (CCValAssign &VA : ArgLocs) {
    if (!VA.isRegLoc() || VA.getLocVT() != MVT::i8 || VA.getValVT() != MVT::i8)
      report_fatal_error(
          "MCS51 MVP backend supports only up to two i8 register arguments");

    Register Reg = MF.addLiveIn(VA.getLocReg(), &MCS51::GPR8RegClass);
    InVals.push_back(DAG.getCopyFromReg(Chain, DL, Reg, MVT::i8));
  }

  return Chain;
}

SDValue MCS51TargetLowering::LowerReturn(
    SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
    const SmallVectorImpl<ISD::OutputArg> &Outs,
    const SmallVectorImpl<SDValue> &OutVals, const SDLoc &DL,
    SelectionDAG &DAG) const {
  SmallVector<CCValAssign, 4> RVLocs;
  CCState CCInfo(CallConv, IsVarArg, DAG.getMachineFunction(), RVLocs,
                 *DAG.getContext());
  CCInfo.AnalyzeReturn(Outs, RetCC_MCS51);

  SDValue Glue;
  SmallVector<SDValue, 4> RetOps(1, Chain);

  for (unsigned I = 0; I < RVLocs.size(); ++I) {
    CCValAssign &VA = RVLocs[I];
    if (!VA.isRegLoc() || VA.getLocReg() != MCS51::A || VA.getLocVT() != MVT::i8)
      report_fatal_error(
          "MCS51 MVP backend supports only a single i8 return value");

    Chain = DAG.getCopyToReg(Chain, DL, VA.getLocReg(), OutVals[I], Glue);
    Glue = Chain.getValue(1);
    RetOps.push_back(DAG.getRegister(VA.getLocReg(), VA.getLocVT()));
  }

  RetOps[0] = Chain;
  if (Glue.getNode())
    RetOps.push_back(Glue);

  return DAG.getNode(MCS51ISD::RET_FLAG, DL, MVT::Other, RetOps);
}
