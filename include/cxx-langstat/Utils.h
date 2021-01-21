#ifndef UTILS_H
#define UTILS_H

#include <iostream>

// #include "llvm/ADT/StringRef.h"

//-----------------------------------------------------------------------------

// For a match whose node is a decl that can be a nameddecl, return its
// human-readable name.
template<typename T>
std::string getMatchDeclName(const Match<T>& Match){
    if(auto n = dyn_cast<clang::NamedDecl>(Match.node)){
        return n->getQualifiedNameAsString(); // includes namespace qualifiers
    } else {
        std::cout << "Decl @ " << Match.location << "cannot be resolved\n";
        return "INVALID";
    }
}

template<typename T>
void printMatches(std::string text, const Matches<T>& Matches){
    std::cout << "\033[33m" << text << ":\033[0m " << Matches.size() << "\n";
    for(auto match : Matches)
        std::cout << getMatchDeclName(match) << " @ " << match.location << "\n";
}

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
                Match<NodeType> m(Location, Node, Result.Context);
                Matches.emplace_back(m);
            } else {
                TimesTypeFailed++;
            }
        }
        if(!Matches.size())
            std::cout << "Cannot get node as type for id \"" << id << "\"\n";
        return Matches;
}

std::string getFileForStatDump(llvm::StringRef InFile);
std::string getFileForPrint(llvm::StringRef S);


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
