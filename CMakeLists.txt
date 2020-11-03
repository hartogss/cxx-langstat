# Info:
# https://llvm.org/docs/CMake.html#embedding-llvm-in-your-project
# https://www.youtube.com/watch?v=kEGQKzhciKc
# and of course cmake.org
# Potential for improvements: peek at https://github.com/firolino/clang-tool

# ---- ----
cmake_minimum_required(VERSION 3.18.3)
# Sets ${PROJECT_NAME}
project(cxx-langstat)
# Define executable to be built
add_executable(${PROJECT_NAME} Langstat.cpp)

# ---- Compiler ----
# Resolved nullptr not found error to go away, but nullptr part of languge since c++11?
set(CMAKE_CXX_STANDARD 14)
# C++ Compiler
message(STATUS "C++ Compiler:" ${CMAKE_CXX_COMPILER})
# No run-time type indentification
set(CMAKE_CXX_FLAGS "-fno-rtti -O0 -Wall -Wextra")
message(STATUS "C++ flags: ${CMAKE_CXX_FLAGS}")

# ---- LLVM, Clang ----
# Loads settings from external project LLVM
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
# Show where LLVM includes are
include_directories(${LLVM_INCLUDE_DIRS})
message(STATUS "Found LLVM include dirs: $${LLVM_INCLUDE_DIRS}")
# What's this for?
add_definitions(${LLVM_DEFINITIONS})
message(STATUS "LLVM Definitions: $${LLVM_DEFINTIONS}")
# Get llvm_libs
execute_process(COMMAND llvm-config --libs
    OUTPUT_VARIABLE llvm_libs
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
string(REPLACE "-l" "" llvm_libs ${llvm_libs})
string(REPLACE " " ";" llvm_libs ${llvm_libs})
message(STATUS "LLVM libs: $${llvm_libs}")
# Set clang_libs
set(clang_libs
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
    clangBasic)
message(STATUS "Clang libs: $${clang_libs}")

# ---- My library & includes ----
# Directory to search for include files
add_subdirectory(include)
#
include_directories(include)
# Directory to search for library/src files
add_subdirectory(lib)
# Specify directory to look for libraries
target_link_directories(${PROJECT_NAME} PUBLIC lib/)

# ---- Link everything together ----
# Link custom libraries
target_link_libraries(${PROJECT_NAME} Extraction Analysis ForStmtAnalysis)
# Link LLVM & Clang libs
target_link_libraries(${PROJECT_NAME} ${llvm_libs} ${clang_libs})

# Extra linking needed on ubuntu, not necessary on macOS(macOS doesn't dislike them either)
target_link_libraries(ForStmtAnalysis Analysis Extraction ${llvm_libs} ${clang_libs})
target_link_libraries(Analysis Extraction ${llvm_libs} ${clang_libs})
target_link_libraries(Extraction ${llvm_libs} ${clang_libs})



# result of llvm-config --libs
# llvm_map_components_to_libnames(llvm_libs XRay  TableGen Symbolize DebugInfoPDB OrcJIT OrcError JITLink ObjectYAML MCA LTO Passes Coroutines ObjCARCOpts Extensions LineEditor LibDriver Interpreter FuzzMutate MCJIT ExecutionEngine RuntimeDyld DWARFLinker DlltoolDriver Option DebugInfoGSYM Coverage XCoreDisassembler XCoreCodeGen XCoreDesc XCoreInfo X86Disassembler X86AsmParser X86CodeGen X86Desc X86Info WebAssemblyDisassembler WebAssemblyCodeGen WebAssemblyDesc WebAssemblyAsmParser WebAssemblyInfo SystemZDisassembler SystemZCodeGen SystemZAsmParser SystemZDesc SystemZInfo SparcDisassembler SparcCodeGen SparcAsmParser SparcDesc SparcInfo RISCVDisassembler RISCVCodeGen RISCVAsmParser RISCVDesc RISCVUtils RISCVInfo PowerPCDisassembler PowerPCCodeGen PowerPCAsmParser PowerPCDesc PowerPCInfo NVPTXCodeGen NVPTXDesc NVPTXInfo MSP430Disassembler MSP430CodeGen MSP430AsmParser MSP430Desc MSP430Info MipsDisassembler MipsCodeGen MipsAsmParser MipsDesc MipsInfo LanaiDisassembler LanaiCodeGen LanaiAsmParser LanaiDesc LanaiInfo HexagonDisassembler HexagonCodeGen HexagonAsmParser HexagonDesc HexagonInfo BPFDisassembler BPFCodeGen BPFAsmParser BPFDesc BPFInfo AVRDisassembler AVRCodeGen AVRAsmParser AVRDesc AVRInfo ARMDisassembler ARMCodeGen ARMAsmParser ARMDesc ARMUtils ARMInfo AMDGPUDisassembler AMDGPUCodeGen MIRParser ipo Instrumentation Vectorize Linker IRReader AsmParser FrontendOpenMP AMDGPUAsmParser AMDGPUDesc AMDGPUUtils AMDGPUInfo AArch64Disassembler MCDisassembler AArch64CodeGen CFGuard GlobalISel SelectionDAG AsmPrinter DebugInfoDWARF CodeGen Target ScalarOpts InstCombine AggressiveInstCombine TransformUtils BitWriter Analysis ProfileData Object TextAPI BitReader Core Remarks BitstreamReader AArch64AsmParser MCParser AArch64Desc MC DebugInfoCodeView DebugInfoMSF BinaryFormat AArch64Utils AArch64Info Support Demangle)
