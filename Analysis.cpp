#include "Extraction.h"
#include "Analysis.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"


#include <iostream>

using namespace clang::ast_matchers;
using namespace clang::tooling;

// should be abstract class
Analysis::Analysis(ClangTool Tool) : _Tool(Tool){

    }
    // step 1: extraction

    //step 2: compute stats
    //step 3: visualization (for later)

    // combine
void Analysis::run(){
    std::cout << "analysis run\n";
    std::string matcherid = "fs";
    StatementMatcher Matcher = forStmt().bind(matcherid);
    int numMatches = extract(matcherid, Matcher, this->_Tool);
    std::cout << "Number of matches " << numMatches << "\n";

}
