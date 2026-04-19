//===-- MCS51MCCodeEmitter.cpp - Convert MCS51 code to bytes -------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "MCS51MCTargetDesc.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/MathExtras.h"

using namespace llvm;

namespace {

class MCS51MCCodeEmitter : public MCCodeEmitter {
public:
  MCS51MCCodeEmitter(const MCInstrInfo &MCII, MCContext &Ctx)
      : MCII(MCII), Ctx(Ctx) {}

  ~MCS51MCCodeEmitter() override = default;

  void encodeInstruction(const MCInst &MI, SmallVectorImpl<char> &CB,
                         SmallVectorImpl<MCFixup> &Fixups,
                         const MCSubtargetInfo &STI) const override;

private:
  static void emitByte(SmallVectorImpl<char> &CB, uint8_t Byte) {
    CB.push_back(static_cast<char>(Byte));
  }

  uint8_t getRegNum(const MCInst &MI, unsigned OperandIndex) const;
  uint8_t getImm8(const MCInst &MI, unsigned OperandIndex) const;

  const MCInstrInfo &MCII;
  MCContext &Ctx;
};

} // namespace

uint8_t MCS51MCCodeEmitter::getRegNum(const MCInst &MI,
                                      unsigned OperandIndex) const {
  const MCOperand &MO = MI.getOperand(OperandIndex);
  if (!MO.isReg()) {
    Ctx.reportError(MI.getLoc(), "expected register operand");
    return 0;
  }

  switch (MO.getReg()) {
  case MCS51::R0:
    return 0;
  case MCS51::R1:
    return 1;
  case MCS51::R2:
    return 2;
  case MCS51::R3:
    return 3;
  case MCS51::R4:
    return 4;
  case MCS51::R5:
    return 5;
  case MCS51::R6:
    return 6;
  case MCS51::R7:
    return 7;
  default:
    Ctx.reportError(MI.getLoc(), "unsupported register for MCS-51 encoder");
    return 0;
  }
}

uint8_t MCS51MCCodeEmitter::getImm8(const MCInst &MI,
                                    unsigned OperandIndex) const {
  const MCOperand &MO = MI.getOperand(OperandIndex);
  if (MO.isImm()) {
    const int64_t Imm = MO.getImm();
    if (!isUInt<8>(Imm) && !isInt<8>(Imm))
      Ctx.reportError(MI.getLoc(), "immediate does not fit in 8 bits");
    return static_cast<uint8_t>(Imm);
  }

  if (MO.isExpr()) {
    Ctx.reportError(MI.getLoc(),
                    "relocations are not supported by the MCS-51 encoder yet");
    return 0;
  }

  Ctx.reportError(MI.getLoc(), "expected immediate operand");
  return 0;
}

void MCS51MCCodeEmitter::encodeInstruction(const MCInst &MI,
                                           SmallVectorImpl<char> &CB,
                                           SmallVectorImpl<MCFixup> &Fixups,
                                           const MCSubtargetInfo &STI) const {
  (void)MCII;
  (void)Fixups;
  (void)STI;

  switch (MI.getOpcode()) {
  case MCS51::MOV8ri:
    emitByte(CB, static_cast<uint8_t>(0x78u + getRegNum(MI, 0)));
    emitByte(CB, getImm8(MI, 1));
    return;
  case MCS51::MOVA_r:
    emitByte(CB, static_cast<uint8_t>(0xE8u + getRegNum(MI, 0)));
    return;
  case MCS51::MOVA_i:
    emitByte(CB, 0x74);
    emitByte(CB, getImm8(MI, 0));
    return;
  case MCS51::MOVB_r:
    emitByte(CB, static_cast<uint8_t>(0x88u + getRegNum(MI, 0)));
    emitByte(CB, 0xF0);
    return;
  case MCS51::MOVB_a:
    emitByte(CB, 0xF5);
    emitByte(CB, 0xF0);
    return;
  case MCS51::MOVB_i:
    emitByte(CB, 0x75);
    emitByte(CB, 0xF0);
    emitByte(CB, getImm8(MI, 0));
    return;
  case MCS51::CLR_A:
    emitByte(CB, 0xE4);
    return;
  case MCS51::RLC_A:
    emitByte(CB, 0x33);
    return;
  case MCS51::RRC_A:
    emitByte(CB, 0x13);
    return;
  case MCS51::MUL_AB:
    emitByte(CB, 0xA4);
    return;
  case MCS51::DIV_AB:
    emitByte(CB, 0x84);
    return;
  case MCS51::MOVR_a:
    emitByte(CB, static_cast<uint8_t>(0xF8u + getRegNum(MI, 0)));
    return;
  case MCS51::MOVR_b:
    emitByte(CB, static_cast<uint8_t>(0xA8u + getRegNum(MI, 0)));
    emitByte(CB, 0xF0);
    return;
  case MCS51::ADDA_r:
    emitByte(CB, static_cast<uint8_t>(0x28u + getRegNum(MI, 0)));
    return;
  case MCS51::ADDA_i:
    emitByte(CB, 0x24);
    emitByte(CB, getImm8(MI, 0));
    return;
  case MCS51::ANLA_r:
    emitByte(CB, static_cast<uint8_t>(0x58u + getRegNum(MI, 0)));
    return;
  case MCS51::ANLA_i:
    emitByte(CB, 0x54);
    emitByte(CB, getImm8(MI, 0));
    return;
  case MCS51::ORLA_r:
    emitByte(CB, static_cast<uint8_t>(0x48u + getRegNum(MI, 0)));
    return;
  case MCS51::ORLA_i:
    emitByte(CB, 0x44);
    emitByte(CB, getImm8(MI, 0));
    return;
  case MCS51::XRLA_r:
    emitByte(CB, static_cast<uint8_t>(0x68u + getRegNum(MI, 0)));
    return;
  case MCS51::XRLA_i:
    emitByte(CB, 0x64);
    emitByte(CB, getImm8(MI, 0));
    return;
  case MCS51::CLR_C:
    emitByte(CB, 0xC3);
    return;
  case MCS51::SUBBA_r:
    emitByte(CB, static_cast<uint8_t>(0x98u + getRegNum(MI, 0)));
    return;
  case MCS51::SUBBA_b:
    emitByte(CB, 0x95);
    emitByte(CB, 0xF0);
    return;
  case MCS51::SUBBA_i:
    emitByte(CB, 0x94);
    emitByte(CB, getImm8(MI, 0));
    return;
  case MCS51::RET:
    emitByte(CB, 0x22);
    return;
  default:
    report_fatal_error("unsupported MCS-51 opcode in MCCodeEmitter");
  }
}

MCCodeEmitter *llvm::createMCS51MCCodeEmitter(const MCInstrInfo &MCII,
                                              MCContext &Ctx) {
  return new MCS51MCCodeEmitter(MCII, Ctx);
}
