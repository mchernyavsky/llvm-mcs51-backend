//===-- MCS51MCAsmInfo.h - MCS51 assembly properties ----------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_MCS51_MCTARGETDESC_MCS51MCASMINFO_H
#define LLVM_LIB_TARGET_MCS51_MCTARGETDESC_MCS51MCASMINFO_H

#include "llvm/MC/MCAsmInfoELF.h"

namespace llvm {

class Triple;

class MCS51MCAsmInfo : public MCAsmInfoELF {
public:
  explicit MCS51MCAsmInfo(const Triple &TT);
};

} // namespace llvm

#endif
