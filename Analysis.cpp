#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"

#include <iostream>

#include "Extraction.h"
#include "Analysis.h"


using namespace clang::ast_matchers;
using namespace clang::tooling;

// should be abstract class?
Analysis::Analysis(ClangTool Tool) : Tool(Tool){
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
    std::cout << "Analysis::run()" << std::endl;
    extract(); //why 'this' not needed?


}
