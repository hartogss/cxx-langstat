#ifndef BASEEXTRACTOR_H
#define BASEEXTRACTOR_H

#include "clang/ASTMatchers/ASTMatchFinder.h"

#include "cxx-langstat/MatchingExtractor.h"

//-----------------------------------------------------------------------------

class BaseExtractor{
public :
    // ctor, dtor
    BaseExtractor(clang::tooling::ClangTool Tool);
    // Responsible for invoking appropriate concrete Extractor
    Matches<clang::Stmt> extract(std::string id, clang::ast_matchers::StatementMatcher Matcher);
    Matches<clang::Decl> extract(std::string id, clang::ast_matchers::DeclarationMatcher Matcher);
private:
    clang::tooling::ClangTool Tool;
};

//-----------------------------------------------------------------------------

#endif // BASEEXTRACTOR_H
