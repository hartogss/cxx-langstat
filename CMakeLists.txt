cmake_minimum_required(VERSION 3.18.3)

project(cxx-langstat) #referenced by ${PROJECT_NAME}

set(CMAKE_CXX_STANDARD 14) #resolved nullptr not found error to go away, but nullptr part of languge since c++11?
set(CMAKE_CXX_FLAGS -fno-rtti)

##
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Set your project compile flags.
# E.g. if using the C++ header files
# you will need to enable C++11 support
# for your compiler.

include_directories(${LLVM_INCLUDE_DIRS})
message(STATUS "Found LLVM include dirs: $${LLVM_INCLUDE_DIRS}")
add_definitions(${LLVM_DEFINITIONS})
message(STATUS "LLVM Definitions: $${LLVM_DEFINTIONS}")
##

#build tool
add_executable(${PROJECT_NAME} Langstat.cpp)

#
add_subdirectory(include/cxx-langstat)
# directory to search for include files
include_directories(include/cxx-langstat)
# not this one, want to have includes not only for target
# target_include_directories(${PROJECT_NAME} PRIVATE include/cxx-langstat)

#
add_subdirectory(lib)
# specify directory to look for libraries
target_link_directories(${PROJECT_NAME} PUBLIC lib/)
# link library to project, i.e. specific files
target_link_libraries(${PROJECT_NAME} Extraction Analysis ForStmtAnalysis)

target_link_libraries(${PROJECT_NAME}
    clangFrontendTool
    clangRewriteFrontend
    clangDynamicASTMatchers
    clangTooling
    clangFrontend
    clangToolingCore
    clangASTMatchers
    clangParse
    clangDriver
    clangSerialization
    clangRewrite
    clangSema
    clangEdit
    clangAnalysis
    clangAST
    clangLex
    clangBasic
)

#yeah, this is not a nice solution, but works for now
llvm_map_components_to_libnames(llvm_libs XRay  TableGen Symbolize DebugInfoPDB OrcJIT OrcError JITLink ObjectYAML MCA LTO Passes Coroutines ObjCARCOpts Extensions LineEditor LibDriver Interpreter FuzzMutate MCJIT ExecutionEngine RuntimeDyld DWARFLinker DlltoolDriver Option DebugInfoGSYM Coverage XCoreDisassembler XCoreCodeGen XCoreDesc XCoreInfo X86Disassembler X86AsmParser X86CodeGen X86Desc X86Info WebAssemblyDisassembler WebAssemblyCodeGen WebAssemblyDesc WebAssemblyAsmParser WebAssemblyInfo SystemZDisassembler SystemZCodeGen SystemZAsmParser SystemZDesc SystemZInfo SparcDisassembler SparcCodeGen SparcAsmParser SparcDesc SparcInfo RISCVDisassembler RISCVCodeGen RISCVAsmParser RISCVDesc RISCVUtils RISCVInfo PowerPCDisassembler PowerPCCodeGen PowerPCAsmParser PowerPCDesc PowerPCInfo NVPTXCodeGen NVPTXDesc NVPTXInfo MSP430Disassembler MSP430CodeGen MSP430AsmParser MSP430Desc MSP430Info MipsDisassembler MipsCodeGen MipsAsmParser MipsDesc MipsInfo LanaiDisassembler LanaiCodeGen LanaiAsmParser LanaiDesc LanaiInfo HexagonDisassembler HexagonCodeGen HexagonAsmParser HexagonDesc HexagonInfo BPFDisassembler BPFCodeGen BPFAsmParser BPFDesc BPFInfo AVRDisassembler AVRCodeGen AVRAsmParser AVRDesc AVRInfo ARMDisassembler ARMCodeGen ARMAsmParser ARMDesc ARMUtils ARMInfo AMDGPUDisassembler AMDGPUCodeGen MIRParser ipo Instrumentation Vectorize Linker IRReader AsmParser FrontendOpenMP AMDGPUAsmParser AMDGPUDesc AMDGPUUtils AMDGPUInfo AArch64Disassembler MCDisassembler AArch64CodeGen CFGuard GlobalISel SelectionDAG AsmPrinter DebugInfoDWARF CodeGen Target ScalarOpts InstCombine AggressiveInstCombine TransformUtils BitWriter Analysis ProfileData Object TextAPI BitReader Core Remarks BitstreamReader AArch64AsmParser MCParser AArch64Desc MC DebugInfoCodeView DebugInfoMSF BinaryFormat AArch64Utils AArch64Info Support Demangle)

target_link_libraries(${PROJECT_NAME} ${llvm_libs})
message(STATUS "LLVM libs: $${llvm_libs}")





# add_subdirectory(lib)
# add_subdirectory(include/cxx-langstat/)
#
# target_include_directories(${PROJECT_NAME} PRIVATE include/cxx-langstat)
#
# target_link_directories(${PROJECT_NAME} PRIVATE include/cxx-langstat)

# target_link_libraries(${PROJECT_NAME} Extraction)
