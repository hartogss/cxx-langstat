#include <iostream>
#include <vector>

#include "llvm/Support/raw_ostream.h"

#include "cxx-langstat/Analyses/StdlibAnalysis2.h"
#include "cxx-langstat/Analyses/TemplateInstantiationAnalysis.h"
#include "cxx-langstat/Utils.h"

using namespace clang;
using namespace clang::ast_matchers;

//-----------------------------------------------------------------------------
// How often were constructs from standard library used (like vector, array,
// map, list, unordered_map, set etc.). Were they used directly as type,
// or as part of another constructs? What behavior can we see when they are
// passed around? What sizes do they occur (#elements, constexpr)?
// Usage in templates and TMP?

StdlibAnalysis2::StdlibAnalysis2(llvm::StringRef InFile,
    clang::ASTContext& Context) :
        Analysis(InFile, Context),
        Context(Context){
}
void StdlibAnalysis2::extract() {

}
void StdlibAnalysis2::analyze(){

}
void StdlibAnalysis2::run(){
    std::cout << "\033[32mRunning standard library analysis:\033[0m" << std::endl;
    auto isAnyStdContainer = hasAnyName( // Copied from UseAutoCheck.cpp
        // Standard library containers
        "array", "vector",
        "forward_list", "list",
        "map", "multimap",
        "set", "multiset",
        "unordered_map", "unordered_multimap",
        "unordered_set", "unordered_multiset",
        "queue", "priority_queue", "stack", "deque",
        // Standard library utilities
        // vocabulary types
        "pair", "tuple",
        // Dynamic memory
        "unique_ptr", "shared_ptr", "weak_ptr"
        );
    TemplateInstantiationAnalysis Helper(InFile, Context, isAnyStdContainer);
    Helper.analyzeFuncInsts=false;
    Helper.analyzeVarInsts=false;
    Helper.run();
}

//-----------------------------------------------------------------------------
