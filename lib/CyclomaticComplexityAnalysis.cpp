// Thanks to Peter Goldsborough for sharing his implementation of CYC/McCabe:
// https://github.com/peter-can-talk/cppnow-2017/blob/master/code/mccabe/mccabe.cpp

#include "llvm/Support/raw_os_ostream.h"
#include "clang/Analysis/CFG.h"

#include <iostream>

#include "cxx-langstat/CyclomaticComplexityAnalysis.h"

using namespace clang::ast_matchers;

//-----------------------------------------------------------------------------

CyclomaticComplexityAnalysis::CyclomaticComplexityAnalysis
(clang::tooling::ClangTool Tool) : Analysis(Tool){

}
void CyclomaticComplexityAnalysis::extract(){
    auto id = "fd";
    auto fDecl = functionDecl().bind(id);
    auto matches = Extr.extract(id, fDecl);
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
        for(auto block = cfg->begin(); block != cfg->end(); block++){
            numEdges += (*block)->succ_size();
        }
        unsigned CYC = numEdges - numNodes + 2; // 2 since #connected components P=1
        clang::FunctionDecl* n = (clang::FunctionDecl*) match.node; //nicer to do this in Extractor::extraction function
        std::cout << n->getNameInfo().getAsString() << " has CYC " << CYC << std::endl;
    }
}
void CyclomaticComplexityAnalysis::analyze(){

}
void CyclomaticComplexityAnalysis::run() {
    std::cout << "Measuring cyclomatic complexity:" << std::endl;
    extract();
}

//-----------------------------------------------------------------------------