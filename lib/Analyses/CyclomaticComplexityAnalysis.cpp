// Thanks to Peter Goldsborough for sharing his implementation of CYC/McCabe:
// https://github.com/peter-can-talk/cppnow-2017/blob/master/code/mccabe/mccabe.cpp

#include "llvm/Support/raw_os_ostream.h"
#include "clang/Analysis/CFG.h"

#include <iostream>

#include "cxx-langstat/Analyses/CyclomaticComplexityAnalysis.h"

using namespace clang::ast_matchers;
using ordered_json = nlohmann::ordered_json;

//-----------------------------------------------------------------------------

void CyclomaticComplexityAnalysis::extract(){
    auto id = "fd";
    auto fDecl = functionDecl(
        isExpansionInMainFile(),
        unless(isImplicit()), // Should not be compiler-generated
        has(compoundStmt()))  // Should be defined, i.e have a body
    .bind(id);
    auto matches = Extractor.extract(*Context, id, fDecl);

    ordered_json fdecls;
    for(auto match : matches){
        // http://clang.llvm.org/doxygen/classclang_1_1CFG.html#details
        // CFG is intraprocedural flow of a statement.
        // Statement can be a function body or a single expression.
        const std::unique_ptr<clang::CFG> cfg =
        clang::CFG::buildCFG(match.node, match.node->getBody(), // probably because body is compoundStmt
            match.ctxt, clang::CFG::BuildOptions()
        );
        // Wanna print the cfgs? Don't, they're ugly.
        // llvm::raw_os_ostream OS(std::cout);
        // cfg->print(OS, clang::LangOptions(), true);
        // Strictly speaking, you'd have to subtract -2 each from numNodes & numEdges
        // because of the LLVM entry & exit block. However, this has no effect on CYC.
        unsigned numNodes = cfg->size();
        unsigned numEdges = 0;
        for(auto block = cfg->begin(); block != cfg->end(); block++)
            numEdges += (*block)->succ_size();
        unsigned CYC = numEdges - numNodes + 2; // 2 since #connected components P=1
        fdecls[getMatchDeclName(match)] = CYC;
    }
    std::cout << fdecls.dump(4) << std::endl;
}

void CyclomaticComplexityAnalysis::run(llvm::StringRef InFile,
    clang::ASTContext& Context) {
        std::cout << "\033[32mMeasuring cyclomatic complexity:\033[0m" << std::endl;
        this->Context = &Context;
        extract();
}

//-----------------------------------------------------------------------------
