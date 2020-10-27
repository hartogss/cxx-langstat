#ifndef EXTRACTION_H
#define EXTRACTION_H

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Tooling/Tooling.h"

//-----------------------------------------------------------------------------


class Extractor : public clang::ast_matchers::MatchFinder::MatchCallback {
public :
    //ctor
    Extractor(std::string id, clang::ast_matchers::StatementMatcher Matcher);
    //
    virtual void run(const clang::ast_matchers::MatchFinder::MatchResult &Result);
    int appearances;
private:
    std::string id;
    clang::ast_matchers::StatementMatcher Matcher;
};

int extract(std::string id, clang::ast_matchers::StatementMatcher Matcher,
    clang::tooling::ClangTool Tool);

//-----------------------------------------------------------------------------


#endif // EXTRACTION_H
