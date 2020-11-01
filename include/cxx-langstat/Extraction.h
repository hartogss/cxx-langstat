#ifndef EXTRACTION_H
#define EXTRACTION_H

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Tooling/Tooling.h"

//-----------------------------------------------------------------------------
// Contains important data extracted from AST
// For now, we extract, given a matcher, #matches and their respective locations.
struct Match {
    Match(unsigned location, const void* node, clang::ASTContext* ctxt);
    ~Match();
    unsigned location;
    const void* node;
    clang::ASTContext* ctxt;
};

using Matches = std::vector<Match>;

//-----------------------------------------------------------------------------

class Extractor : public clang::ast_matchers::MatchFinder::MatchCallback {
public :
    // ctor, dtor
    Extractor();
    Extractor(clang::tooling::ClangTool Tool);
    ~Extractor();
    // Run when match is found after extract call with Matcher
    virtual void run(const clang::ast_matchers::MatchFinder::MatchResult &Result);
    // Call this method to get data from AST
    Matches extract(std::string id, clang::ast_matchers::StatementMatcher Matcher);
    Matches extract(std::string id, clang::ast_matchers::DeclarationMatcher Matcher);
private:
    // Resets state
    void resetState();
    //
    clang::tooling::ClangTool Tool;
    std::string matcherid;
    Matches matches;

};


#endif // EXTRACTION_H
