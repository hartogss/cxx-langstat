#include "Extraction.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"


#include <iostream>

using namespace clang::ast_matchers;
using namespace clang::tooling;

// need analysis object since we want other analysis to inherit interface
class Analysis {
public:
    Analysis(ClangTool Tool) : _Tool(Tool){

    }
    // step 1: extraction

    //step 2: compute stats
    //step 3: visualization (for later)

    // combine
    void run(){
        std::cout << "analysis run\n";
        std::string matcherid = "fs";
        StatementMatcher Matcher = forStmt().bind(matcherid);
        int numMatches = extract(matcherid, Matcher, this->_Tool);
        std::cout << "Number of matches " << numMatches << "\n";

    }

    // std::string name;
    ClangTool _Tool;

};

llvm::cl::OptionCategory ClangStatCategory("clang-stat options");
llvm::cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);
llvm::cl::extrahelp MoreHelp("\nMore help text coming soon...\n");

int main(int argc, const char** argv){
    // parses all options that command-line tools have in common
    CommonOptionsParser Parser(argc, argv, ClangStatCategory);

    // Print all file name & locations
    for (auto SourceFilePath : Parser.getSourcePathList()){
        std::cout << SourceFilePath << std::endl;
    }

    ClangTool Tool(Parser.getCompilations(), Parser.getSourcePathList());

    Analysis A(Tool);
    A.run();



    return 0;
}
