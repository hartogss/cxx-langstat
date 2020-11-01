#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"

#include <iostream>
#include <vector>
#include <numeric>

#include "cxx-langstat/Extraction.h"
#include "cxx-langstat/ForStmtAnalysis.h"

// namespaces
using namespace clang::ast_matchers;
using namespace clang::tooling; // ClangTool

// Constructs matcher that exactly matches for-loops with depth d (nesting depth)
StatementMatcher constructForMatcher(std::string Name, int d){
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

// Constructs matcher that exactly matches mixed loops with depth d (nesting depth)
StatementMatcher constructMixedMatcher(std::string Name, int d){
    StatementMatcher NumOfDescendantsAtLeastD = anything();
    int i=1;
    while(i<d){
        NumOfDescendantsAtLeastD = hasDescendant(stmt(anyOf(
            forStmt(NumOfDescendantsAtLeastD), whileStmt(NumOfDescendantsAtLeastD), doStmt(NumOfDescendantsAtLeastD))));
        i++;
    }
    StatementMatcher NumOfDescendantsAtLeastDPlus1 = hasDescendant(stmt(anyOf(
            forStmt(NumOfDescendantsAtLeastD), whileStmt(NumOfDescendantsAtLeastD), doStmt(NumOfDescendantsAtLeastD))));
    StatementMatcher NumOfDescendantsExactlyD = allOf(NumOfDescendantsAtLeastD, unless(NumOfDescendantsAtLeastDPlus1));
    StatementMatcher IsOuterMostLoop = unless(hasAncestor(stmt(anyOf(forStmt(), whileStmt(), doStmt()))));
    StatementMatcher LoopHasExactlyDepthD = allOf(IsOuterMostLoop, NumOfDescendantsExactlyD);

    return stmt(anyOf(
        forStmt(LoopHasExactlyDepthD),
        whileStmt(LoopHasExactlyDepthD),
        doStmt(LoopHasExactlyDepthD))).bind(Name);
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
    // Bind is necessary to retrieve information about the match like location etc.
    // Without bind the match is still registered, thus we can still count #matches, but nothing else

    // Depth analysis
    // auto matches = this->Extr.extract("fs1", forStmt(unless(hasAncestor(forStmt()))).bind("fs1"));
    StatementMatcher IsOuterMostLoop = unless(hasAncestor(stmt(anyOf(forStmt(), whileStmt(), doStmt()))));
    auto matches = this->Extr.extract("fs1", stmt(anyOf(
        forStmt(IsOuterMostLoop),
        whileStmt(IsOuterMostLoop),
        doStmt(IsOuterMostLoop))).bind("fs1"));

    unsigned TopLevelForLoops = matches.size();
    std::cout << "#Top-level for loops:" << TopLevelForLoops << std::endl;
    std::vector<Matches> Data;
    unsigned ForLoopsFound = 0;
    for (int i=1; i<=this->MaxDepth; i++){
        StatementMatcher Matcher = constructMixedMatcher("fs", i);
        auto matches = this->Extr.extract("fs", Matcher);
        ForLoopsFound += matches.size();
        Data.emplace_back(matches);
        if(ForLoopsFound == TopLevelForLoops) // stop searching if all possible loops have been found
            break;
    }
    if(ForLoopsFound < TopLevelForLoops)
        std::cout << "Some for loops deeper than max_depth have not been analyzed" << std::endl;
    analyzeDepth(matches, Data);

    // Analysis of prevalence of different loop statement, i.e. comparing for, while etc.
    auto ForMatches = this->Extr.extract("fs1", forStmt().bind("fs1"));
    auto WhileMatches = this->Extr.extract("ws1", whileStmt().bind("ws1"));
    auto DoWhileMatches = this->Extr.extract("ds1", doStmt().bind("ds1"));
    analyzeLoopPrevalences(ForMatches, WhileMatches, DoWhileMatches);
}
//step 2: compute stats
void ForStmtAnalysis::analyzeDepth(Matches matches, std::vector<Matches> Data){
    unsigned TopLevelForLoops = matches.size();
    unsigned depth = 1;
    for (auto matches : Data){
        unsigned d = matches.size();
        if (d!=0)
            std::cout << d << "/" << TopLevelForLoops << " of depth " << depth << " @ lines: ";
            for (auto m : matches){
                std::cout << m.location << " ";
            }
            std::cout << std::endl;
        depth++;
    }
}
void ForStmtAnalysis::analyzeLoopPrevalences(Matches fs, Matches ws, Matches ds){
    unsigned total = fs.size() + ws.size() + ds.size();
    std::cout << fs.size() << "/" << total << " are for loops" << std::endl;
    std::cout << ws.size() << "/" << total << " are while loops" << std::endl;
    std::cout << ds.size() << "/" << total << " are do-while loops" << std::endl;

}
//step 3: visualization (for later)
// combine
void ForStmtAnalysis::run(){
    extract(); //why 'this' not needed?
}
