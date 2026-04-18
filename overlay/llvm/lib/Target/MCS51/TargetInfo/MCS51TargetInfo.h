//===-- MCS51TargetInfo.h - MCS51 target info ------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_MCS51_TARGETINFO_MCS51TARGETINFO_H
#define LLVM_LIB_TARGET_MCS51_TARGETINFO_MCS51TARGETINFO_H

namespace llvm {

class Target;

Target &getTheMCS51Target();

} // namespace llvm

#endif
