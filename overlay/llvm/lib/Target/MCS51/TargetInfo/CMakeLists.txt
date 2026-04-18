add_llvm_component_library(LLVMMCS51Info
  MCS51TargetInfo.cpp

  LINK_COMPONENTS
  MC
  Support

  ADD_TO_COMPONENT
  MCS51
)
