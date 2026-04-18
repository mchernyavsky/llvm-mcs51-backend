//===-- MCS51ISelDAGToDAG.cpp - MCS51 DAG pattern selector ---------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "MCS51.h"
#include "MCS51Subtarget.h"
#include "MCS51TargetMachine.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/Support/Debug.h"

using namespace llvm;

#define DEBUG_TYPE "mcs51-isel"

namespace {

class MCS51DAGToDAGISel : public SelectionDAGISel {
public:
  explicit MCS51DAGToDAGISel(MCS51TargetMachine &TM, CodeGenOptLevel OL)
      : SelectionDAGISel(TM, OL) {}

  bool runOnMachineFunction(MachineFunction &MF) override {
    return SelectionDAGISel::runOnMachineFunction(MF);
  }

  void Select(SDNode *Node) override;

private:
  bool selectBinaryI8(SDNode *Node, unsigned RegOpc, unsigned ImmOpc,
                      bool IsCommutable);
  void SelectCode(SDNode *Node);
};

class MCS51DAGToDAGISelLegacy : public SelectionDAGISelLegacy {
public:
  static char ID;

  MCS51DAGToDAGISelLegacy(MCS51TargetMachine &TM, CodeGenOptLevel OL)
      : SelectionDAGISelLegacy(ID, std::make_unique<MCS51DAGToDAGISel>(TM, OL)) {}
};

} // namespace

#define GET_DAGISEL_BODY MCS51DAGToDAGISel
#include "MCS51GenDAGISel.inc"

char MCS51DAGToDAGISelLegacy::ID;

INITIALIZE_PASS(MCS51DAGToDAGISelLegacy, DEBUG_TYPE,
                "MCS51 DAG->DAG Pattern Instruction Selection", false, false)

FunctionPass *llvm::createMCS51ISelDag(MCS51TargetMachine &TM,
                                       CodeGenOptLevel OL) {
  return new MCS51DAGToDAGISelLegacy(TM, OL);
}

bool MCS51DAGToDAGISel::selectBinaryI8(SDNode *Node, unsigned RegOpc,
                                       unsigned ImmOpc, bool IsCommutable) {
  SDLoc DL(Node);
  SDValue LHS = Node->getOperand(0);
  SDValue RHS = Node->getOperand(1);

  auto selectImm = [&](SDValue Other, ConstantSDNode *C) {
    SDValue TargetImm =
        CurDAG->getTargetConstant(C->getSExtValue(), DL, MVT::i8);
    CurDAG->SelectNodeTo(Node, ImmOpc, MVT::i8, Other, TargetImm);
    return true;
  };

  if (auto *C = dyn_cast<ConstantSDNode>(RHS))
    return selectImm(LHS, C);

  if (IsCommutable)
    if (auto *C = dyn_cast<ConstantSDNode>(LHS))
      return selectImm(RHS, C);

  CurDAG->SelectNodeTo(Node, RegOpc, MVT::i8, LHS, RHS);
  return true;
}

void MCS51DAGToDAGISel::Select(SDNode *Node) {
  if (Node->isMachineOpcode()) {
    Node->setNodeId(-1);
    return;
  }

  if (Node->getOpcode() == MCS51ISD::CMP) {
    SDLoc DL(Node);
    SDValue LHS = Node->getOperand(0);
    SDValue RHS = Node->getOperand(1);
    SDValue Flags = Node->getOperand(2);
    EVT ResultVT = Node->getValueType(0);
    const bool IsBoolResult = ResultVT == MVT::i1;
    const unsigned RROpc = IsBoolResult ? MCS51::CMP1rr : MCS51::CMP8rr;
    const unsigned RIOpc = IsBoolResult ? MCS51::CMP1ri : MCS51::CMP8ri;
    const unsigned IROpc = IsBoolResult ? MCS51::CMP1ir : MCS51::CMP8ir;

    if (auto *RHSImm = dyn_cast<ConstantSDNode>(RHS)) {
      SDValue TargetImm =
          CurDAG->getTargetConstant(RHSImm->getSExtValue(), DL, MVT::i8);
      CurDAG->SelectNodeTo(Node, RIOpc, ResultVT, LHS, TargetImm, Flags);
      return;
    }

    if (auto *LHSImm = dyn_cast<ConstantSDNode>(LHS)) {
      SDValue TargetImm =
          CurDAG->getTargetConstant(LHSImm->getSExtValue(), DL, MVT::i8);
      CurDAG->SelectNodeTo(Node, IROpc, ResultVT, TargetImm, RHS, Flags);
      return;
    }

    CurDAG->SelectNodeTo(Node, RROpc, ResultVT, LHS, RHS, Flags);
    return;
  }

  if (Node->getOpcode() == MCS51ISD::SHL) {
    CurDAG->SelectNodeTo(Node, MCS51::SHL8ri, MVT::i8, Node->getOperand(0),
                         Node->getOperand(1));
    return;
  }

  if (Node->getOpcode() == MCS51ISD::LSHR) {
    CurDAG->SelectNodeTo(Node, MCS51::LSHR8ri, MVT::i8, Node->getOperand(0),
                         Node->getOperand(1));
    return;
  }

  if (Node->getSimpleValueType(0) == MVT::i8) {
    switch (Node->getOpcode()) {
    case ISD::ADD:
      if (selectBinaryI8(Node, MCS51::ADD8rr, MCS51::ADD8ri, true))
        return;
      break;
    case ISD::SUB:
      if (selectBinaryI8(Node, MCS51::SUB8rr, MCS51::SUB8ri, false))
        return;
      break;
    case ISD::AND:
      if (selectBinaryI8(Node, MCS51::AND8rr, MCS51::AND8ri, true))
        return;
      break;
    case ISD::MUL:
      if (selectBinaryI8(Node, MCS51::MUL8rr, MCS51::MUL8ri, true))
        return;
      break;
    case ISD::UDIV:
      if (isa<ConstantSDNode>(Node->getOperand(1)))
        break;
      CurDAG->SelectNodeTo(Node, MCS51::DIV8rr, MVT::i8, Node->getOperand(0),
                           Node->getOperand(1));
      return;
    case ISD::UREM:
      if (isa<ConstantSDNode>(Node->getOperand(1)))
        break;
      CurDAG->SelectNodeTo(Node, MCS51::REM8rr, MVT::i8, Node->getOperand(0),
                           Node->getOperand(1));
      return;
    case ISD::OR:
      if (selectBinaryI8(Node, MCS51::OR8rr, MCS51::OR8ri, true))
        return;
      break;
    case ISD::XOR:
      if (selectBinaryI8(Node, MCS51::XOR8rr, MCS51::XOR8ri, true))
        return;
      break;
    default:
      break;
    }
  }

  SelectCode(Node);
}
