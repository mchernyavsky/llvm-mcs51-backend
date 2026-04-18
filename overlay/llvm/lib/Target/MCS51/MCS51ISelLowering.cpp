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

namespace {

uint8_t getCompareFlags(ISD::CondCode CC) {
  switch (CC) {
  case ISD::SETEQ:
    return MCS51CmpFlags::UseXorNonZero | MCS51CmpFlags::InvertResult;
  case ISD::SETNE:
    return MCS51CmpFlags::UseXorNonZero;
  case ISD::SETULT:
    return MCS51CmpFlags::None;
  case ISD::SETUGE:
    return MCS51CmpFlags::InvertResult;
  case ISD::SETUGT:
    return MCS51CmpFlags::SwapOperands;
  case ISD::SETULE:
    return MCS51CmpFlags::SwapOperands | MCS51CmpFlags::InvertResult;
  default:
    report_fatal_error(
        "MCS51 MVP backend supports only i8 eq/ne and unsigned ordering comparisons");
  }
}

SDValue emitCompareMaterialization(SelectionDAG &DAG, const SDLoc &DL,
                                   EVT ResultVT, SDValue LHS, SDValue RHS,
                                   uint8_t Flags) {
  return DAG.getNode(MCS51ISD::CMP, DL, ResultVT, LHS, RHS,
                     DAG.getTargetConstant(Flags, DL, MVT::i8));
}

SDValue unwrapSetCC(SDValue Value) {
  while (Value.getOpcode() == ISD::TRUNCATE ||
         Value.getOpcode() == ISD::AssertZext ||
         Value.getOpcode() == ISD::AssertSext)
    Value = Value.getOperand(0);

  return Value.getOpcode() == ISD::SETCC ? Value : SDValue();
}

} // namespace

MCS51TargetLowering::MCS51TargetLowering(const TargetMachine &TM,
                                         const MCS51Subtarget &STI)
    : TargetLowering(TM) {
  addRegisterClass(MVT::i8, &MCS51::GPR8RegClass);
  addRegisterClass(MVT::i1, &MCS51::GPR1RegClass);

  computeRegisterProperties(STI.getRegisterInfo());
  setStackPointerRegisterToSaveRestore(MCS51::SP);
  setBooleanContents(ZeroOrOneBooleanContent);

  setOperationAction(ISD::ADD, MVT::i8, Legal);
  setOperationAction(ISD::SUB, MVT::i8, Legal);
  setOperationAction(ISD::AND, MVT::i8, Legal);
  setOperationAction(ISD::OR, MVT::i8, Legal);
  setOperationAction(ISD::XOR, MVT::i8, Legal);
  setOperationAction(ISD::SETCC, MVT::i8, Custom);
  setOperationAction(ISD::SELECT, MVT::i8, Custom);
  setOperationAction(ISD::SELECT_CC, MVT::i8, Custom);
  setOperationAction(ISD::ZERO_EXTEND, MVT::i8, Custom);
}

EVT MCS51TargetLowering::getSetCCResultType(const DataLayout &DL,
                                            LLVMContext &Context,
                                            EVT VT) const {
  (void)DL;
  (void)Context;
  (void)VT;
  return MVT::i1;
}

SDValue MCS51TargetLowering::LowerOperation(SDValue Op,
                                            SelectionDAG &DAG) const {
  switch (Op.getOpcode()) {
  case ISD::SETCC:
    return LowerSetCC(Op, DAG);
  case ISD::SELECT:
    return LowerSelect(Op, DAG);
  case ISD::SELECT_CC:
    return LowerSelectCC(Op, DAG);
  case ISD::ZERO_EXTEND:
    return LowerZeroExtend(Op, DAG);
  default:
    llvm_unreachable("unimplemented operand");
  }
}

SDValue MCS51TargetLowering::LowerSetCC(SDValue Op,
                                        SelectionDAG &DAG) const {
  if (Op.getOperand(0).getValueType() != MVT::i8 ||
      Op.getOperand(1).getValueType() != MVT::i8)
    report_fatal_error(
        "MCS51 MVP backend supports only i8 eq/ne and unsigned ordering comparisons");

  return emitCompareMaterialization(
      DAG, SDLoc(Op), Op.getValueType(), Op.getOperand(0), Op.getOperand(1),
      getCompareFlags(cast<CondCodeSDNode>(Op.getOperand(2))->get()));
}

SDValue MCS51TargetLowering::LowerSelect(SDValue Op,
                                         SelectionDAG &DAG) const {
  if (Op.getValueType() != MVT::i8) {
    report_fatal_error(
        "MCS51 MVP backend supports only i8 select materialization");
  }

  const auto *TrueC = dyn_cast<ConstantSDNode>(Op.getOperand(1));
  const auto *FalseC = dyn_cast<ConstantSDNode>(Op.getOperand(2));
  if (TrueC == nullptr || FalseC == nullptr)
    report_fatal_error(
        "MCS51 MVP backend supports only constant i8 select results");

  SDValue SetCC = unwrapSetCC(Op.getOperand(0));
  if (!SetCC || SetCC.getOperand(0).getValueType() != MVT::i8 ||
      SetCC.getOperand(1).getValueType() != MVT::i8)
    report_fatal_error(
        "MCS51 MVP backend supports only selects driven by i8 setcc");

  uint8_t Flags = getCompareFlags(cast<CondCodeSDNode>(SetCC.getOperand(2))->get());
  if (TrueC->getZExtValue() == 0 && FalseC->getZExtValue() == 1)
    Flags ^= MCS51CmpFlags::InvertResult;
  else if (TrueC->getZExtValue() != 1 || FalseC->getZExtValue() != 0)
    report_fatal_error(
        "MCS51 MVP backend supports only select materialization to 0/1");

  return emitCompareMaterialization(DAG, SDLoc(Op), Op.getValueType(),
                                    SetCC.getOperand(0), SetCC.getOperand(1),
                                    Flags);
}

SDValue MCS51TargetLowering::LowerSelectCC(SDValue Op,
                                           SelectionDAG &DAG) const {
  if (Op.getValueType() != MVT::i8 || Op.getOperand(0).getValueType() != MVT::i8 ||
      Op.getOperand(1).getValueType() != MVT::i8)
    report_fatal_error(
        "MCS51 MVP backend supports only i8 select_cc materialization");

  const auto *TrueC = dyn_cast<ConstantSDNode>(Op.getOperand(2));
  const auto *FalseC = dyn_cast<ConstantSDNode>(Op.getOperand(3));
  if (TrueC == nullptr || FalseC == nullptr)
    report_fatal_error(
        "MCS51 MVP backend supports only constant i8 select_cc results");

  uint8_t Flags = getCompareFlags(cast<CondCodeSDNode>(Op.getOperand(4))->get());
  if (TrueC->getZExtValue() == 0 && FalseC->getZExtValue() == 1)
    Flags ^= MCS51CmpFlags::InvertResult;
  else if (TrueC->getZExtValue() != 1 || FalseC->getZExtValue() != 0)
    report_fatal_error(
        "MCS51 MVP backend supports only select_cc materialization to 0/1");

  return emitCompareMaterialization(DAG, SDLoc(Op), Op.getValueType(),
                                    Op.getOperand(0), Op.getOperand(1), Flags);
}

SDValue MCS51TargetLowering::LowerZeroExtend(SDValue Op,
                                             SelectionDAG &DAG) const {
  if (Op.getValueType() != MVT::i8)
    report_fatal_error(
        "MCS51 MVP backend supports only i8 boolean materialization");

  SDValue SetCC = unwrapSetCC(Op.getOperand(0));
  if (Op.getOperand(0).getValueType() != MVT::i1 || !SetCC)
    report_fatal_error(
        "MCS51 MVP backend supports only zext(setcc) boolean materialization");
  if (SetCC.getOperand(0).getValueType() != MVT::i8 ||
      SetCC.getOperand(1).getValueType() != MVT::i8)
    report_fatal_error(
        "MCS51 MVP backend supports only i8 eq/ne and unsigned ordering comparisons");

  return emitCompareMaterialization(
      DAG, SDLoc(Op), Op.getValueType(), SetCC.getOperand(0),
      SetCC.getOperand(1),
      getCompareFlags(cast<CondCodeSDNode>(SetCC.getOperand(2))->get()));
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
