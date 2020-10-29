#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"

#include <iostream>

#include "Extraction.h"
#include "Analysis.h"


using namespace clang::ast_matchers;
using namespace clang::tooling;

// should be abstract class?
Analysis::Analysis(ClangTool Tool) : Extr(Extractor(Tool)){
    std::cout<<"Analysis ctor"<<std::endl;
}
Analysis::~Analysis(){
    std::cout<<"Analysis dtor"<<std::endl;
}
// step 1: extraction
void Analysis::extract() {
}
//step 2: compute stats
void Analysis::analyze(){
}
//step 3: visualization (for later)

// combine
void Analysis::run(){
    std::cout << "Empty Analysis run, stopping." << std::endl;
    extract(); //why 'this' not needed?
}
