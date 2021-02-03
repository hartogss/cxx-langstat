# Info:
# https://llvm.org/docs/CMake.html#embedding-llvm-in-your-project
# https://www.youtube.com/watch?v=kEGQKzhciKc
# and of course cmake.org
# Potential for improvements: peek at https://github.com/firolino/clang-tool
# colored output with ninja: # https://medium.com/@alasher/colored-c-compiler-output-with-ninja-clang-gcc-10bfe7f2b949

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
find_package(LLVM 11 REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
# Show where LLVM includes are
include_directories(${LLVM_INCLUDE_DIRS})
message(STATUS "Found LLVM include dirs: ${LLVM_INCLUDE_DIRS}")
# Get llvm_libs
execute_process(COMMAND llvm-config --libs
    OUTPUT_VARIABLE llvm_libs
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# find_package(clang REQUIRED CONFIG)
# message(STATUS "Using ClangConfig.cmake in: ${clang_DIR}")

string(REPLACE "-l" "" llvm_libs ${llvm_libs})
string(REPLACE " " ";" llvm_libs ${llvm_libs})
# message(STATUS "LLVM libs: $${llvm_libs}")
# Set clang_libs
set(clang_libs
    clangFrontendTool clangRewriteFrontend clangDynamicASTMatchers
    clangTooling clangFrontend clangToolingCore clangASTMatchers
    clangParse clangDriver clangSerialization clangRewrite clangSema
    clangEdit clangAnalysis clangAST clangLex clangBasic
)
# message(STATUS "Clang libs: $${clang_libs}")

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
target_link_libraries(${PROJECT_NAME}
    BaseExtractor MatchingExtractor
    AnalysisRegistry AnalysisList
    Utils Stats
    AlgorithmLibraryAnalysis
    CyclomaticComplexityAnalysis
    LoopDepthAnalysis
    LoopKindAnalysis
    MoveSemanticsAnalysis
    StdlibAnalysis2
    TemplateInstantiationAnalysis
    TemplateParameterAnalysis
    UsingAnalysis
    VariableTemplateAnalysis
    )
# Link LLVM & Clang libs
target_link_libraries(${PROJECT_NAME} ${llvm_libs} ${clang_libs})
add_library(Driver Driver.cpp)
target_link_libraries(${PROJECT_NAME} Driver)

# Extra linking needed on ubuntu, not necessary on macOS(macOS doesn't dislike them either)
target_link_libraries(BaseExtractor MatchingExtractor ${llvm_libs} ${clang_libs})
target_link_libraries(MatchingExtractor ${llvm_libs} ${clang_libs})
target_link_libraries(CyclomaticComplexityAnalysis BaseExtractor MatchingExtractor ${llvm_libs} ${clang_libs})
target_link_libraries(LoopDepthAnalysis BaseExtractor MatchingExtractor ${llvm_libs} ${clang_libs})




# Testing, not yet already
# clangtidy libs I could once need
set(clang_tidy_libs
clangTidy;clangTidyAndroidModule;clangTidyAbseilModule;clangTidyBoostModule;
clangTidyBugproneModule;clangTidyCERTModule;clangTidyCppCoreGuidelinesModule;
clangTidyDarwinModule;clangTidyFuchsiaModule;clangTidyGoogleModule;clangTidyHICPPModule;
clangTidyLinuxKernelModule;clangTidyLLVMModule;clangTidyLLVMLibcModule;clangTidyMiscModule;
clangTidyModernizeModule;clangTidyMPIModule;clangTidyObjCModule;clangTidyOpenMPModule;
clangTidyPerformanceModule;clangTidyPortabilityModule;clangTidyReadabilityModule;
clangTidyZirconModule;clangTidyPlugin;clangTidyMain;clangTidyUtils;
)
# message(STATUS "Clang-tidy libs: ${clang_tidy_libs}")
# target_link_libraries(${PROJECT_NAME} ${clang_tidy_libs})
