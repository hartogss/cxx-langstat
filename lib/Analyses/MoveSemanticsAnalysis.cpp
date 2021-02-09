#include "cxx-langstat/Analyses/MoveSemanticsAnalysis.h"

using namespace clang::ast_matchers;
using ojson = nlohmann::ordered_json;

void MoveSemanticsAnalysis::CopyOrMoveAnalyzer::analyzeFeatures() {
    auto m = callExpr(isExpansionInMainFile(),
        hasAnyArgument(expr(hasType(recordType())).bind("expr")));
    auto Res = Extractor.extract2(*Context, m);
    auto CallExpr = getASTNodes<clang::CallExpr>(Res, "expr");


}
void MoveSemanticsAnalysis::CopyOrMoveAnalyzer::processFeatures(ojson j) {

}
