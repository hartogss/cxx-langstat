#ifndef EXTRACTION_H
#define EXTRACTION_H

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Tooling/Tooling.h"

//-----------------------------------------------------------------------------


class Extractor : public clang::ast_matchers::MatchFinder::MatchCallback {
public :
    Extractor();
    //ctor
    Extractor(clang::tooling::ClangTool Tool);
    // Run when match is found after extract call with Matcher
    virtual void run(const clang::ast_matchers::MatchFinder::MatchResult &Result);
    // Call this method to get data from AST
    int extract(std::string id, clang::ast_matchers::StatementMatcher Matcher);
    int NumMatches;
private:
    // Resets state, possibly better to do that on extract() call
    unsigned resetState();
    //
    clang::tooling::ClangTool Tool;
    std::string matcherid;
};


#endif // EXTRACTION_H
