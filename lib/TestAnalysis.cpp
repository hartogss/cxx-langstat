#include <iostream>
#include <vector>

#include "cxx-langstat/TestAnalysis.h"
#include "clang-tidy/ClangTidy.h"
#include "clang-tidy/tool/ClangTidyMain.h"


using namespace clang::ast_matchers;
using namespace clang::tooling; // ClangTool

//-----------------------------------------------------------------------------

// TODO: why parent ctor?
TestAnalysis::TestAnalysis(ClangTool Tool) : Analysis(Tool) {
}
// step 1: extraction
void TestAnalysis::extract() {
}
//step 2: compute stats
void TestAnalysis::analyze(){

}
//step 3: visualization (for later)
// combine
void TestAnalysis::run(){
    //test decl Analysis
    DeclarationMatcher fd = functionDecl().bind("fd");
    auto fdmatches = this->Extr.extract("fd", fd);
    std::cout<< "#fdecls:" << fdmatches.size() << std::endl;
    for(auto m: fdmatches){
        clang::FunctionDecl* n = (clang::FunctionDecl*) m.node; //nicer to do this in Extractor::extraction function
        std::cout << n->getNameInfo().getAsString() << "@" << m.location << std::endl;
    }

    //test call Analysis
    StatementMatcher ce = callExpr().bind("ce");
    auto cematches = this->Extr.extract("ce", ce);
    std::cout<< "#callexprs:" << cematches.size() << std::endl;
    for(auto m: cematches){
        clang::CallExpr* n = (clang::CallExpr*) m.node; //nicer to do this in Extractor::extraction function
        std::cout << n->getDirectCallee()->getNameInfo().getAsString() << "@" << m.location << std::endl;
    }
}

//-----------------------------------------------------------------------------

void runclangtidy(){
    // int a = 2;
    // char* one = "-checks=-*,modernize-use-trailing-return-type";
    // char* two =  "../test/LoopStructure/BasicLoops.cpp";
    // const char** in = {one, two};
    // clang::tidy::clangTidyMain(a, in);
}