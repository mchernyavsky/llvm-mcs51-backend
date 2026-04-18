//===-- MCS51MCTargetDesc.h - MCS51 target descriptions --------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_MCS51_MCTARGETDESC_MCS51MCTARGETDESC_H
#define LLVM_LIB_TARGET_MCS51_MCTARGETDESC_MCS51MCTARGETDESC_H

#include <memory>
#include "llvm/Support/DataTypes.h"

namespace llvm {

class MCAsmInfo;
class MCAsmBackend;
class MCCodeEmitter;
class MCContext;
class MCInstrInfo;
class MCObjectTargetWriter;
class MCRegisterInfo;
class MCSubtargetInfo;
class MCInstPrinter;
class MCTargetOptions;
class Target;
class Triple;

MCCodeEmitter *createMCS51MCCodeEmitter(const MCInstrInfo &MCII,
                                        MCContext &Ctx);

MCAsmBackend *createMCS51AsmBackend(const Target &T,
                                    const MCSubtargetInfo &STI,
                                    const MCRegisterInfo &MRI,
                                    const MCTargetOptions &Options);

std::unique_ptr<MCObjectTargetWriter> createMCS51ELFObjectWriter(uint8_t OSABI);

} // namespace llvm

#define GET_REGINFO_ENUM
#include "MCS51GenRegisterInfo.inc"

#define GET_INSTRINFO_ENUM
#define GET_INSTRINFO_MC_HELPER_DECLS
#include "MCS51GenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "MCS51GenSubtargetInfo.inc"

#endif
