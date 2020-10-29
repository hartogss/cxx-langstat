#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"

#include <iostream>
#include <vector>
#include <numeric>

#include "Extraction.h"
#include "ForStmtAnalysis.h"

// namespaces
using namespace clang::ast_matchers;
using namespace clang::tooling; // ClangTool

// Constructs matcher that exactly matches for-loops with depth d (nesting depth)
StatementMatcher constructMatcher(int d, std::string Name){
    StatementMatcher AtLeastDepth = anything();
    int i=1;
    while(i<d){
        AtLeastDepth = hasDescendant(forStmt(AtLeastDepth));
        i++;
    }
    StatementMatcher EnsureIsOuterMostLoop = unless(hasAncestor(forStmt()));
    StatementMatcher AtLeastDepthPlus1 = forStmt(EnsureIsOuterMostLoop, hasDescendant(forStmt(AtLeastDepth)));
    AtLeastDepth = forStmt(EnsureIsOuterMostLoop, AtLeastDepth);
    return forStmt(AtLeastDepth, unless(AtLeastDepthPlus1)).bind(Name);
}

// TODO: why parent ctor?
ForStmtAnalysis::ForStmtAnalysis(ClangTool Tool, int MaxDepthOption) :
    Analysis(Tool),
    MaxDepth(MaxDepthOption) {
    std::cout<<"FSA ctor"<<std::endl;
}
ForStmtAnalysis::~ForStmtAnalysis() {
    std::cout<<"FSA dtor"<<std::endl;
}
// step 1: extraction
void ForStmtAnalysis::extract() {
    std::string matcherid = "fs";
    std::vector<int> Data;
    // Bind is necessary to retrieve information about the match like location etc.
    // Without bind the match is still registered, thus we can still count #matches, but nothing else
    unsigned TotalForLoops = this->Extr.extract("fs1", forStmt(unless(hasAncestor(forStmt()))).bind("fs1")).numMatches;
    std::cout << "#Independent for-loops:" << TotalForLoops << std::endl;
    unsigned ForLoopsFound = 0;
    for (int i=1; i<=this->MaxDepth; i++){
        StatementMatcher Matcher = constructMatcher(i, matcherid);
        unsigned DataPoint = this->Extr.extract(matcherid, Matcher).numMatches;
        ForLoopsFound += DataPoint;
        Data.emplace_back(DataPoint);
        if(ForLoopsFound == TotalForLoops) // stop searching if all possible loops have been found
            break;
    }
    if(ForLoopsFound < TotalForLoops)
        std::cout << "Some for loops deeper than max_depth have not been analyzed" << std::endl;
    analyze(Data);
}
//step 2: compute stats
void ForStmtAnalysis::analyze(std::vector<int> Data){
    unsigned DetectedForLoops = std::accumulate(Data.begin(), Data.end(), 0);
    unsigned depth = 1;
    for (int d : Data){
        if (d!=0)
            std::cout << d << "/" << DetectedForLoops << " of depth " << depth << std::endl;
        depth++;
    }
}
//step 3: visualization (for later)
// combine
void ForStmtAnalysis::run(){
    extract(); //why 'this' not needed?
}
