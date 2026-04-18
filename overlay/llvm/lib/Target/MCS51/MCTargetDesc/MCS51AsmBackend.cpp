//===-- MCS51AsmBackend.cpp - MCS51 assembler backend --------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "MCS51MCTargetDesc.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCTargetOptions.h"
#include "llvm/Support/Endian.h"
#include "llvm/Support/MathExtras.h"

using namespace llvm;

namespace {

class MCS51AsmBackend : public MCAsmBackend {
public:
  explicit MCS51AsmBackend(uint8_t OSABI)
      : MCAsmBackend(llvm::endianness::little), OSABI(OSABI) {}

  ~MCS51AsmBackend() override = default;

  void applyFixup(const MCFragment &F, const MCFixup &Fixup,
                  const MCValue &Target, MutableArrayRef<char> Data,
                  uint64_t Value, bool IsResolved) override {
    maybeAddReloc(F, Fixup, Target, Value, IsResolved);

    if (!Value)
      return;

    const MCFixupKindInfo Info = getFixupKindInfo(Fixup.getKind());
    const unsigned NumBytes = alignTo(Info.TargetOffset + Info.TargetSize, 8) / 8;
    const unsigned Offset = Fixup.getOffset();

    assert(Offset + NumBytes <= Data.size() && "invalid fixup offset");

    Value <<= Info.TargetOffset;
    for (unsigned I = 0; I != NumBytes; ++I)
      Data[Offset + I] |= static_cast<char>((Value >> (I * 8)) & 0xFF);
  }

  std::unique_ptr<MCObjectTargetWriter> createObjectTargetWriter() const override {
    return createMCS51ELFObjectWriter(OSABI);
  }

  MCFixupKindInfo getFixupKindInfo(MCFixupKind Kind) const override {
    if (Kind < FirstTargetFixupKind)
      return MCAsmBackend::getFixupKindInfo(Kind);
    llvm_unreachable("MCS-51 target fixups are not implemented");
  }

  bool writeNopData(raw_ostream &OS, uint64_t Count,
                    const MCSubtargetInfo *STI) const override {
    (void)STI;
    for (uint64_t I = 0; I != Count; ++I)
      OS.write("\x00", 1);
    return true;
  }

private:
  uint8_t OSABI;
};

} // namespace

MCAsmBackend *llvm::createMCS51AsmBackend(const Target &T,
                                          const MCSubtargetInfo &STI,
                                          const MCRegisterInfo &MRI,
                                          const MCTargetOptions &Options) {
  (void)T;
  (void)MRI;
  (void)Options;
  const uint8_t OSABI =
      MCELFObjectTargetWriter::getOSABI(STI.getTargetTriple().getOS());
  return new MCS51AsmBackend(OSABI);
}
