#ifndef EXTRACTION_H
#define EXTRACTION_H

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Tooling/Tooling.h"

//-----------------------------------------------------------------------------
// Contains important data extracted from AST
// For now, we extract, given a matcher, #matches and their respective locations.
struct Gist {
    Gist();
    ~Gist();
    unsigned numMatches;
    std::vector<unsigned> locations;
};

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
    Gist extract(std::string id, clang::ast_matchers::StatementMatcher Matcher);
private:
    // Resets state
    void resetState();
    //
    clang::tooling::ClangTool Tool;
    std::string matcherid;
    Gist gist;

};


#endif // EXTRACTION_H
