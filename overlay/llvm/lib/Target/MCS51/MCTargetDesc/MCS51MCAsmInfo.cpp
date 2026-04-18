//===-- MCS51MCAsmInfo.cpp - MCS51 assembly properties -------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "MCS51MCAsmInfo.h"

using namespace llvm;

MCS51MCAsmInfo::MCS51MCAsmInfo(const Triple &TT) {
  CodePointerSize = 2;
  CalleeSaveStackSlotSize = 1;
  IsLittleEndian = true;
  CommentString = ";";
}
