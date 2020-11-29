#ifndef UTILS_H
#define UTILS_H

//-----------------------------------------------------------------------------

std::string getDeclName(Match<clang::Decl> Match);

void printStatistics(std::string text, Matches<clang::Decl> matches);

template<typename NodeType>
Matches<NodeType> getASTNodes(std::vector<clang::ast_matchers::MatchFinder::MatchResult> Results, std::string id){
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
    // We now know that id must be bound to some node in some result in the Results
    // vector, so below in can only fail if it cannot be gotten as the desired type
    unsigned TimesTypeFailed = 0;
    for(auto Result : Results){
        if(const NodeType* Node = Result.Nodes.getNodeAs<NodeType>(id)) {
            unsigned Location = Result.Context->getFullLoc(Node->getBeginLoc()).getLineNumber();
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

//-----------------------------------------------------------------------------

#endif // UTILS_H
