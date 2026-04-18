add_llvm_component_group(MCS51)

set(LLVM_TARGET_DEFINITIONS MCS51.td)

tablegen(LLVM MCS51GenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM MCS51GenCallingConv.inc -gen-callingconv)
tablegen(LLVM MCS51GenDAGISel.inc -gen-dag-isel)
tablegen(LLVM MCS51GenInstrInfo.inc -gen-instr-info)
tablegen(LLVM MCS51GenRegisterInfo.inc -gen-register-info)
tablegen(LLVM MCS51GenSubtargetInfo.inc -gen-subtarget)

add_public_tablegen_target(MCS51CommonTableGen)

add_llvm_target(MCS51CodeGen
  MCS51AsmPrinter.cpp
  MCS51FrameLowering.cpp
  MCS51ISelDAGToDAG.cpp
  MCS51ISelLowering.cpp
  MCS51InstrInfo.cpp
  MCS51MCInstLower.cpp
  MCS51RegisterInfo.cpp
  MCS51Subtarget.cpp
  MCS51TargetMachine.cpp

  LINK_COMPONENTS
  AsmPrinter
  CodeGen
  CodeGenTypes
  Core
  MC
  MCS51Desc
  MCS51Info
  SelectionDAG
  Support
  Target

  ADD_TO_COMPONENT
  MCS51
)

add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)
