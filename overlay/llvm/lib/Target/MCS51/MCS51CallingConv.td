//===-- MCS51CallingConv.td - Calling conventions for MCS51 -*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

def RetCC_MCS51 : CallingConv<[
  CCIfType<[i8], CCAssignToReg<[A]>>
]>;

def CC_MCS51 : CallingConv<[
  CCIfType<[i8], CCAssignToReg<[R7, R6]>>
]>;

def CSR_NoRegs : CalleeSavedRegs<(add)>;
