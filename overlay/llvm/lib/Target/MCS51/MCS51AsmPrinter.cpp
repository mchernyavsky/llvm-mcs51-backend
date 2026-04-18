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
  static bool isGPRReg(Register Reg) {
    return MCS51::GPR8RegClass.contains(Reg) || MCS51::GPR1RegClass.contains(Reg);
  }

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

  void emitMoveBToReg(Register Dst) {
    emitMCInst(MCS51::MOVR_b, {MCOperand::createReg(Dst)});
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

  void emitLoadB(const MachineOperand &Src) {
    if (Src.isReg()) {
      emitMCInst(MCS51::MOVB_r, {MCOperand::createReg(Src.getReg())});
      return;
    }
    if (Src.isImm()) {
      emitMCInst(MCS51::MOVB_i, {MCOperand::createImm(Src.getImm())});
      return;
    }
    report_fatal_error("Unsupported MCS51 B register load source");
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

  void emitComparePseudo(const MachineInstr *MI) {
    const uint8_t Flags = static_cast<uint8_t>(MI->getOperand(3).getImm());
    const MachineOperand &LHS = MI->getOperand(1);
    const MachineOperand &RHS = MI->getOperand(2);
    if (Flags & MCS51CmpFlags::UseXorNonZero) {
      emitLoadA(LHS);
      if (RHS.isReg())
        emitMCInst(MCS51::XRLA_r, {lowerPseudoOperand(RHS)});
      else if (RHS.isImm())
        emitMCInst(MCS51::XRLA_i, {lowerPseudoOperand(RHS)});
      else
        report_fatal_error("Unsupported MCS51 equality comparison RHS");
      emitMCInst(MCS51::ADDA_i, {MCOperand::createImm(0xFF)});
    } else {
      const MachineOperand &First =
          (Flags & MCS51CmpFlags::SwapOperands) ? RHS : LHS;
      const MachineOperand &Second =
          (Flags & MCS51CmpFlags::SwapOperands) ? LHS : RHS;

      emitLoadA(First);
      emitMCInst(MCS51::CLR_C, {});
      if (Second.isReg())
        emitMCInst(MCS51::SUBBA_r, {lowerPseudoOperand(Second)});
      else if (Second.isImm())
        emitMCInst(MCS51::SUBBA_i, {lowerPseudoOperand(Second)});
      else
        report_fatal_error("Unsupported MCS51 comparison RHS");
    }
    emitMCInst(MCS51::CLR_A, {});
    emitMCInst(MCS51::RLC_A, {});
    if (Flags & MCS51CmpFlags::InvertResult)
      emitMCInst(MCS51::XRLA_i, {MCOperand::createImm(1)});
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
    if (Src.getReg() == MCS51::A && isGPRReg(Dst.getReg())) {
      emitMoveAToReg(Dst.getReg());
      return;
    }
    if (isGPRReg(Dst.getReg()) && isGPRReg(Src.getReg())) {
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
  case MCS51::SHL8ri: {
    emitLoadA(MI->getOperand(1));
    const int64_t ShiftCount = MI->getOperand(2).getImm();
    if (ShiftCount < 0 || ShiftCount > 7)
      report_fatal_error("MCS51 supports only constant i8 shifts in the range 0..7");
    for (int64_t I = 0; I < ShiftCount; ++I) {
      emitMCInst(MCS51::CLR_C, {});
      emitMCInst(MCS51::RLC_A, {});
    }
    emitMoveAToReg(MI->getOperand(0).getReg());
    return;
  }
  case MCS51::MUL8rr:
  case MCS51::MUL8ri:
    emitLoadA(MI->getOperand(1));
    emitLoadB(MI->getOperand(2));
    emitMCInst(MCS51::MUL_AB, {});
    emitMoveAToReg(MI->getOperand(0).getReg());
    return;
  case MCS51::DIV8rr:
    emitLoadA(MI->getOperand(1));
    emitLoadB(MI->getOperand(2));
    emitMCInst(MCS51::DIV_AB, {});
    emitMoveAToReg(MI->getOperand(0).getReg());
    return;
  case MCS51::REM8rr:
    emitLoadA(MI->getOperand(1));
    emitLoadB(MI->getOperand(2));
    emitMCInst(MCS51::DIV_AB, {});
    emitMoveBToReg(MI->getOperand(0).getReg());
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
  case MCS51::CMP8rr:
  case MCS51::CMP8ri:
  case MCS51::CMP8ir:
  case MCS51::CMP1rr:
  case MCS51::CMP1ri:
  case MCS51::CMP1ir:
    emitComparePseudo(MI);
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
