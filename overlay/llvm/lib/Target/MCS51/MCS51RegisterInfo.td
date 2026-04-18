//===-- MCS51RegisterInfo.td - MCS51 register definitions ---*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

class MCS51Reg<bits<16> Num, string Name, list<string> Alt = []>
    : Register<Name> {
  let HWEncoding = Num;
  let Namespace = "MCS51";
  let AltNames = Alt;
  let DwarfNumbers = [Num];
}

def A   : MCS51Reg<0,  "a">;
def B   : MCS51Reg<1,  "b">;
def R0  : MCS51Reg<2,  "r0">;
def R1  : MCS51Reg<3,  "r1">;
def R2  : MCS51Reg<4,  "r2">;
def R3  : MCS51Reg<5,  "r3">;
def R4  : MCS51Reg<6,  "r4">;
def R5  : MCS51Reg<7,  "r5">;
def R6  : MCS51Reg<8,  "r6">;
def R7  : MCS51Reg<9,  "r7">;
def PSW : MCS51Reg<10, "psw">;
def SP  : MCS51Reg<11, "sp">;
def PC  : MCS51Reg<12, "pc">;

def GPR8 : RegisterClass<"MCS51", [i8], 8, (add R7, R6, R5, R4, R3, R2, R1, R0)>;
def ACC8 : RegisterClass<"MCS51", [i8], 8, (add A)> {
  let CopyCost = -1;
}
