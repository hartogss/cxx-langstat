# Info:
# https://llvm.org/docs/CMake.html#embedding-llvm-in-your-project
# https://www.youtube.com/watch?v=kEGQKzhciKc
# and of course cmake.org
# Potential for improvements: peek at https://github.com/firolino/clang-tool
# colored output with ninja: https://medium.com/@alasher/colored-c-compiler-output-with-ninja-clang-gcc-10bfe7f2b949

# ---- ----
cmake_minimum_required(VERSION 3.18.3)
# Sets ${PROJECT_NAME}
project(cxx-langstat)

# ---- Compiler ----
# C++17
set(CMAKE_CXX_STANDARD_REQUIRED 17)
# set(CMAKE_CXX_STANDARD 17) # I prefer to use -std=c++17
# C++ Compiler
message(STATUS "C++ Compiler:" ${CMAKE_CXX_COMPILER})
set(CLANGXX ${CMAKE_CXX_COMPILER})
# No run-time type indentification
# -fvisibility=hidden to stop linker complaining about weak symbols when linking clang-tidy
# -fcolor-diagnostics to enable clang colored output when using ninja
set(CMAKE_CXX_FLAGS "-std=c++17 -fno-rtti -g -pedantic -O0 -Wall -Wextra -fcolor-diagnostics")
message(STATUS "C++ flags: ${CMAKE_CXX_FLAGS}")

# Define executable to be built
# When you add_executable() before set(CMAKE_CXX_STANDARD)
add_executable(${PROJECT_NAME} Runner.cpp)

# ---- LLVM, Clang ----
# Loads settings from external project LLVM
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
# Show where LLVM includes are
include_directories(${LLVM_INCLUDE_DIRS})
message(STATUS "Found LLVM include dirs: ${LLVM_INCLUDE_DIRS}")

# Find llvm-config
message(STATUS "llvm definitions: ${LLVM_DEFINITIONS}")
message(STATUS "llvmtools binary dir ${LLVM_TOOLS_BINARY_DIR}")
find_program(llvm-config "llvm-config" PATHS ${LLVM_TOOLS_BINARY_DIR} NO_DEFAULT_PATH REQUIRED)
message(STATUS "Using llvm-config in ${llvm-config}")
# Get llvm_libs
execute_process(COMMAND ${llvm-config} --libs # --system-libs--ignore-libllvm
    OUTPUT_VARIABLE llvm_libs
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# find_package(clang REQUIRED CONFIG)
# message(STATUS "Using ClangConfig.cmake in: ${clang_DIR}")

message(STATUS "LLVM libs: ${llvm_libs}")
string(REPLACE "-l" "" llvm_libs ${llvm_libs})
string(REPLACE " " ";" llvm_libs ${llvm_libs})
message(STATUS "LLVM libs: ${llvm_libs}")

# Set clang_libs
# https://jonasdevlieghere.com/understanding-the-clang-ast/
set(clang_libs
    clangFrontendTool clangRewriteFrontend clangDynamicASTMatchers
    clangTooling clangFrontend clangToolingCore clangASTMatchers
    clangParse clangDriver clangSerialization clangRewrite clangSema
    clangEdit clangAnalysis clangAST clangLex clangBasic
)
# message(STATUS "Clang libs: $${clang_libs}")

# --- Where to look for includes ----
# Includes from cxx-langstat thus are #include "cxx-langstat/<header>"
include_directories(include)

# Directory to search for library/src files
add_subdirectory(lib)

add_library(Driver Driver.cpp)

# ---- Link everything together ----
# Order matters! Rule of thumb: the lower something is in this list, the more
# code relies on its symbols and thus the more important it is.
# The linker will learn about unresolved symbols from higher file first, and thus
# have then the most opportunities to resolve them.
# On macOS, these rules don't seem to be that important.
target_link_libraries(${PROJECT_NAME}
    -pthread
    Driver
    Stats
    AnalysisRegistry
    AnalysisList
    Analyses
    MatchingExtractor
    ${llvm_libs} ${clang_libs}
)

# For info about this and lit.site.cfg.py.in and lit.cfg.py see
# https://medium.com/@mshockwave/using-llvm-lit-out-of-tree-5cddada85a78
configure_file(test/lit.site.cfg.py.in test/lit.site.cfg.py)
# Execute this using ninja check
add_custom_target(check
  COMMAND lit test -v -s
DEPENDS ${PROJECT_NAME})
