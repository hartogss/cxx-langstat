#include <iostream>
#include "clang/ASTMatchers/ASTMatchFinder.h"

#include "cxx-langstat/MatchingExtractor.h"

//-----------------------------------------------------------------------------

std::string getDeclName(Match<clang::Decl> Match){
    if(auto n = dyn_cast<clang::NamedDecl>(Match.node)){
        return n->getNameAsString();
        // vs. getQualifiednameAsString() ?
    } else {
        std::cout << "Decl @ " << Match.location << "cannot be resolved\n";
        return "INVALID";
    }
}

void printStatistics(std::string text, Matches<clang::Decl> matches){
    std::cout << "\033[33m" << text << ":\033[0m " << matches.size() << "\n";
    for(auto m : matches)
        std::cout << getDeclName(m) << " @ " << m.location << std::endl;
}

//-----------------------------------------------------------------------------
