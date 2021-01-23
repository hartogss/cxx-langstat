#include "cxx-langstat/Options.h"

#include "clang/Tooling/CompilationDatabase.h"

//-----------------------------------------------------------------------------

int CXXLangstatMain(std::vector<std::string> InputFiles,
    std::vector<std::string> OutputFiles, Stage Stage, std::string Analyses,
    std::string BuildPath,
    std::unique_ptr<clang::tooling::CompilationDatabase> db);

//-----------------------------------------------------------------------------
