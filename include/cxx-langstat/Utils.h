#ifndef UTILS_H
#define UTILS_H

#include <iostream>

// #include "llvm/ADT/StringRef.h"

//-----------------------------------------------------------------------------
// For a match whose node is a decl that can be a nameddecl, return its
// human-readable name.
template<typename T>
std::string getMatchDeclName(const Match<T>& Match){
    if(auto n = dyn_cast<clang::NamedDecl>(Match.Node)){
        return n->getQualifiedNameAsString(); // includes namespace qualifiers
    } else {
        std::cout << "Decl @ " << Match.Location << "cannot be resolved\n";
        return "INVALID";
    }
}
// For a list containing matches that are derived from NamedDecl, print them.
template<typename T>
void printMatches(std::string text, const Matches<T>& Matches){
    std::cout << "\033[33m" << text << ":\033[0m " << Matches.size() << "\n";
    for(auto match : Matches)
        std::cout << getMatchDeclName(match) << " @ " << match.Location << "\n";
}

//-----------------------------------------------------------------------------
// Given a vector of Matchresults and an id, extract the actual nodes in the
// Clang AST by returning pointers to them.
// We thus assume that the ASTContext holding those nodes has a longer lifetime
// than an analysis using them.
template<typename NodeType>
Matches<NodeType>
getASTNodes(std::vector<clang::ast_matchers::MatchFinder::MatchResult> Results,
    std::string id){
        Matches<NodeType> Matches;
        unsigned TimesidNotBound = 0;
        for(auto Result : Results){
            auto Mapping = Result.Nodes.getMap();
            if(!Mapping.count(id))
                TimesidNotBound++;
        }
        if(Results.size()==TimesidNotBound){
            std::cout << "No nodes were bound to given id \"" << id << "\"\n";
            return Matches;
        }
        // We now know that id must be bound to some node in some result in the
        // Results vector, so below in can only fail if it cannot be gotten as
        // the desired type.
        unsigned TimesTypeFailed = 0;
        for(auto Result : Results){
            if(const NodeType* Node = Result.Nodes.getNodeAs<NodeType>(id)) {
                unsigned Location = Result.Context->getFullLoc(
                    Node->getBeginLoc()).getLineNumber();
                Match<NodeType> m(Location, Node);
                Matches.emplace_back(m);
            } else {
                TimesTypeFailed++;
            }
        }
        if(!Matches.size())
            std::cout << "Cannot get node as type for id \"" << id << "\"\n";
        return Matches;
}

//-----------------------------------------------------------------------------

template<typename T>
void removeDuplicateMatches(Matches<T>& Matches){
    if(!std::is_sorted(Matches.begin(), Matches.end())){
        std::sort(Matches.begin(), Matches.end());
    }
    unsigned s = Matches.size();
    for(unsigned idx=0; idx<s-1; idx++){
        if(Matches[idx]==Matches[idx+1]){
            Matches.erase(Matches.begin()+idx+1);
            idx--;
            s--;
        }
    }
}

//-----------------------------------------------------------------------------

#endif // UTILS_H
