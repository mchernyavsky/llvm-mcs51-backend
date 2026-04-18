//===-- MCS51AsmPrinter.cpp - MCS51 LLVM assembly writer -----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/MCS51InstPrinter.h"
#include "MCS51MCInstLower.h"
#include "MCS51TargetMachine.h"
#include "TargetInfo/MCS51TargetInfo.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

#define DEBUG_TYPE "asm-printer"

namespace {

class MCS51AsmPrinter : public AsmPrinter {
public:
  MCS51AsmPrinter(TargetMachine &TM, std::unique_ptr<MCStreamer> Streamer)
      : AsmPrinter(TM, std::move(Streamer), ID) {}

  StringRef getPassName() const override { return "MCS51 Assembly Printer"; }
  void emitInstruction(const MachineInstr *MI) override;

  static char ID;

private:
  static bool isGPR8(Register Reg) { return MCS51::GPR8RegClass.contains(Reg); }

  static MCOperand lowerPseudoOperand(const MachineOperand &MO) {
    if (MO.isReg())
      return MCOperand::createReg(MO.getReg());
    if (MO.isImm())
      return MCOperand::createImm(MO.getImm());
    report_fatal_error("Unsupported MCS51 pseudo operand kind");
  }

  void emitMCInst(unsigned Opcode, ArrayRef<MCOperand> Operands) {
    MCInst Inst;
    Inst.setOpcode(Opcode);
    for (const MCOperand &Op : Operands)
      Inst.addOperand(Op);
    EmitToStreamer(*OutStreamer, Inst);
  }

  void emitMoveAToReg(Register Dst) {
    emitMCInst(MCS51::MOVR_a, {MCOperand::createReg(Dst)});
  }

  void emitLoadA(const MachineOperand &Src) {
    if (Src.isReg()) {
      if (Src.getReg() == MCS51::A)
        return;
      emitMCInst(MCS51::MOVA_r, {MCOperand::createReg(Src.getReg())});
      return;
    }
    if (Src.isImm()) {
      emitMCInst(MCS51::MOVA_i, {MCOperand::createImm(Src.getImm())});
      return;
    }
    report_fatal_error("Unsupported MCS51 accumulator load source");
  }

  void emitBinaryPseudo(const MachineInstr *MI, unsigned AccRegOpcode,
                        unsigned AccImmOpcode) {
    emitLoadA(MI->getOperand(1));
    const MachineOperand &RHS = MI->getOperand(2);
    if (RHS.isReg())
      emitMCInst(AccRegOpcode, {MCOperand::createReg(RHS.getReg())});
    else if (RHS.isImm())
      emitMCInst(AccImmOpcode, {MCOperand::createImm(RHS.getImm())});
    else
      report_fatal_error("Unsupported MCS51 binary pseudo RHS");
    emitMoveAToReg(MI->getOperand(0).getReg());
  }
};

} // namespace

void MCS51AsmPrinter::emitInstruction(const MachineInstr *MI) {
  switch (MI->getOpcode()) {
  case MCS51::ADJCALLSTACKDOWN:
  case MCS51::ADJCALLSTACKUP:
    return;

  case TargetOpcode::COPY: {
    const MachineOperand &Dst = MI->getOperand(0);
    const MachineOperand &Src = MI->getOperand(1);
    if (!Dst.isReg() || !Src.isReg())
      report_fatal_error("Malformed COPY in MCS51 backend");
    if (Dst.getReg() == Src.getReg())
      return;
    if (Dst.getReg() == MCS51::A) {
      emitLoadA(Src);
      return;
    }
    if (Src.getReg() == MCS51::A && isGPR8(Dst.getReg())) {
      emitMoveAToReg(Dst.getReg());
      return;
    }
    if (isGPR8(Dst.getReg()) && isGPR8(Src.getReg())) {
      emitLoadA(Src);
      emitMoveAToReg(Dst.getReg());
      return;
    }
    report_fatal_error("Unsupported COPY in MCS51 backend");
  }

  case MCS51::MOV8rr:
    emitLoadA(MI->getOperand(1));
    emitMoveAToReg(MI->getOperand(0).getReg());
    return;
  case MCS51::ADD8rr:
  case MCS51::ADD8ri:
    emitBinaryPseudo(MI, MCS51::ADDA_r, MCS51::ADDA_i);
    return;
  case MCS51::AND8rr:
  case MCS51::AND8ri:
    emitBinaryPseudo(MI, MCS51::ANLA_r, MCS51::ANLA_i);
    return;
  case MCS51::OR8rr:
  case MCS51::OR8ri:
    emitBinaryPseudo(MI, MCS51::ORLA_r, MCS51::ORLA_i);
    return;
  case MCS51::XOR8rr:
  case MCS51::XOR8ri:
    emitBinaryPseudo(MI, MCS51::XRLA_r, MCS51::XRLA_i);
    return;
  case MCS51::SUB8rr:
  case MCS51::SUB8ri:
    emitLoadA(MI->getOperand(1));
    emitMCInst(MCS51::CLR_C, {});
    if (MI->getOperand(2).isReg())
      emitMCInst(MCS51::SUBBA_r, {lowerPseudoOperand(MI->getOperand(2))});
    else if (MI->getOperand(2).isImm())
      emitMCInst(MCS51::SUBBA_i, {lowerPseudoOperand(MI->getOperand(2))});
    else
      report_fatal_error("Unsupported MCS51 subtraction RHS");
    emitMoveAToReg(MI->getOperand(0).getReg());
    return;
  }

  MCS51MCInstLower MCInstLowering(OutContext, *this);
  MCInst TmpInst;
  MCInstLowering.Lower(MI, TmpInst);
  EmitToStreamer(*OutStreamer, TmpInst);
}

char MCS51AsmPrinter::ID = 0;

INITIALIZE_PASS(MCS51AsmPrinter, "mcs51-asm-printer", "MCS51 Assembly Printer",
                false, false)

extern "C" LLVM_ABI LLVM_EXTERNAL_VISIBILITY void
LLVMInitializeMCS51AsmPrinter() {
  RegisterAsmPrinter<MCS51AsmPrinter> X(getTheMCS51Target());
}
