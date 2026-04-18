//===-- MCS51ELFObjectWriter.cpp - MCS51 ELF object writer ---------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "MCS51MCTargetDesc.h"
#include "llvm/BinaryFormat/ELF.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCValue.h"

using namespace llvm;

namespace {

class MCS51ELFObjectWriter : public MCELFObjectTargetWriter {
public:
  explicit MCS51ELFObjectWriter(uint8_t OSABI)
      : MCELFObjectTargetWriter(/*Is64Bit=*/false, OSABI, ELF::EM_8051,
                                /*HasRelocationAddend=*/true) {}

  ~MCS51ELFObjectWriter() override = default;

protected:
  unsigned getRelocType(const MCFixup &Fixup, const MCValue &Target,
                        bool IsPCRel) const override {
    (void)Fixup;
    (void)Target;
    (void)IsPCRel;
    return 0;
  }
};

} // namespace

std::unique_ptr<MCObjectTargetWriter>
llvm::createMCS51ELFObjectWriter(uint8_t OSABI) {
  return std::make_unique<MCS51ELFObjectWriter>(OSABI);
}
