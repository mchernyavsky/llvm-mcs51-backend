//===-- MCS51MCTargetDesc.cpp - MCS51 target descriptions ----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "MCS51MCTargetDesc.h"
#include "MCS51InstPrinter.h"
#include "MCS51MCAsmInfo.h"
#include "TargetInfo/MCS51TargetInfo.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCELFStreamer.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Compiler.h"

using namespace llvm;

#define GET_INSTRINFO_MC_DESC
#define ENABLE_INSTR_PREDICATE_VERIFIER
#include "MCS51GenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "MCS51GenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "MCS51GenRegisterInfo.inc"

static MCInstrInfo *createMCS51MCInstrInfo() {
  auto *X = new MCInstrInfo();
  InitMCS51MCInstrInfo(X);
  return X;
}

static MCRegisterInfo *createMCS51MCRegisterInfo(const Triple &TT) {
  auto *X = new MCRegisterInfo();
  InitMCS51MCRegisterInfo(X, MCS51::PC);
  return X;
}

static MCAsmInfo *createMCS51MCAsmInfo(const MCRegisterInfo &MRI,
                                       const Triple &TT,
                                       const MCTargetOptions &Options) {
  return new MCS51MCAsmInfo(TT);
}

static MCSubtargetInfo *createMCS51MCSubtargetInfo(const Triple &TT,
                                                   StringRef CPU,
                                                   StringRef FS) {
  return createMCS51MCSubtargetInfoImpl(TT, CPU, CPU, FS);
}

static MCInstPrinter *createMCS51MCInstPrinter(const Triple &TT,
                                               unsigned SyntaxVariant,
                                               const MCAsmInfo &MAI,
                                               const MCInstrInfo &MII,
                                               const MCRegisterInfo &MRI) {
  if (SyntaxVariant == 0)
    return new MCS51InstPrinter(MAI, MII, MRI);
  return nullptr;
}

static MCStreamer *createMCS51MCStreamer(const Triple &TT, MCContext &Context,
                                         std::unique_ptr<MCAsmBackend> &&MAB,
                                         std::unique_ptr<MCObjectWriter> &&OW,
                                         std::unique_ptr<MCCodeEmitter> &&Emitter) {
  return createELFStreamer(Context, std::move(MAB), std::move(OW),
                           std::move(Emitter));
}

extern "C" LLVM_ABI LLVM_EXTERNAL_VISIBILITY void LLVMInitializeMCS51TargetMC() {
  Target &T = getTheMCS51Target();
  TargetRegistry::RegisterMCAsmInfo(T, createMCS51MCAsmInfo);
  TargetRegistry::RegisterMCInstrInfo(T, createMCS51MCInstrInfo);
  TargetRegistry::RegisterMCRegInfo(T, createMCS51MCRegisterInfo);
  TargetRegistry::RegisterMCSubtargetInfo(T, createMCS51MCSubtargetInfo);
  TargetRegistry::RegisterMCInstPrinter(T, createMCS51MCInstPrinter);
  TargetRegistry::RegisterMCCodeEmitter(T, createMCS51MCCodeEmitter);
  TargetRegistry::RegisterELFStreamer(T, createMCS51MCStreamer);
  TargetRegistry::RegisterMCAsmBackend(T, createMCS51AsmBackend);
}
