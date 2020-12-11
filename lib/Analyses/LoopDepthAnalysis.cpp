#include <iostream>
#include <vector>

#include "cxx-langstat/Analyses/LoopDepthAnalysis.h"

using namespace clang::ast_matchers;

//-----------------------------------------------------------------------------

// Constructs matcher that exactly matches for-loops with depth d (nesting depth)
StatementMatcher constructForMatcher(std::string Name, int d){
    StatementMatcher AtLeastDepth = anything();
    int i=1;
    while(i<d){
        AtLeastDepth = hasDescendant(forStmt(AtLeastDepth));
        i++;
    }
    StatementMatcher EnsureIsOuterMostLoop = unless(hasAncestor(forStmt()));
    StatementMatcher AtLeastDepthPlus1 =
        forStmt(EnsureIsOuterMostLoop, hasDescendant(forStmt(AtLeastDepth)));
    AtLeastDepth = forStmt(EnsureIsOuterMostLoop, AtLeastDepth);
    return forStmt(AtLeastDepth, unless(AtLeastDepthPlus1)).bind(Name);
}

// Constructs matcher that exactly matches mixed loops with depth d (nesting depth)
StatementMatcher constructMixedMatcher(std::string Name, int d){
    auto loopStmt = [](auto m){
        return stmt(
            isExpansionInMainFile(),
            anyOf(forStmt(m), whileStmt(m), doStmt(m), cxxForRangeStmt(m)));
    };
    StatementMatcher NumOfDescendantsAtLeastD = anything();
    int i=1;
    while(i<d){
        NumOfDescendantsAtLeastD = hasDescendant(
            loopStmt(NumOfDescendantsAtLeastD));
        i++;
    }
    StatementMatcher NumOfDescendantsAtLeastDPlus1 =
        hasDescendant(loopStmt(NumOfDescendantsAtLeastD));
    StatementMatcher NumOfDescendantsExactlyD =
        allOf(NumOfDescendantsAtLeastD, unless(NumOfDescendantsAtLeastDPlus1));
    StatementMatcher IsOuterMostLoop =
        unless(hasAncestor(loopStmt(anything())));
    StatementMatcher LoopHasExactlyDepthD =
        allOf(IsOuterMostLoop, NumOfDescendantsExactlyD);
    return loopStmt(LoopHasExactlyDepthD).bind(Name);
}

//-----------------------------------------------------------------------------

LoopDepthAnalysis::LoopDepthAnalysis() : Analysis(){
            std::cout<<"FSA ctor"<<std::endl;
}
LoopDepthAnalysis::~LoopDepthAnalysis(){
    std::cout << "destory" << std::endl;
}
// step 1: extraction
void LoopDepthAnalysis::extract(clang::ASTContext& Context) {
    // Bind is necessary to retrieve information about the match like location etc.
    // Without bind the match is still registered, thus we can still count #matches, but nothing else

    // Depth analysis
    // auto matches = this->Extr.extract("fs1", forStmt(unless(hasAncestor(forStmt()))).bind("fs1"));
    StatementMatcher IsOuterMostLoop =
        unless(hasAncestor(stmt(anyOf(forStmt(), whileStmt(), doStmt()))));
    auto matches = this->Extractor.extract(Context, "fs1", stmt(
        isExpansionInMainFile(),
        anyOf(
            forStmt(IsOuterMostLoop),
            whileStmt(IsOuterMostLoop),
            doStmt(IsOuterMostLoop)))
    .bind("fs1"));

    unsigned TopLevelForLoops = matches.size();
    std::cout << "#Top-level for loops:" << TopLevelForLoops << std::endl;
    std::vector<Matches<clang::Stmt>> Data;
    unsigned ForLoopsFound = 0;
    for (int i=1; i<=this->MaxDepth; i++){
        StatementMatcher Matcher = constructMixedMatcher("fs", i);
        auto matches = this->Extractor.extract(Context, "fs", Matcher);
        ForLoopsFound += matches.size();
        Data.emplace_back(matches);
        if(ForLoopsFound == TopLevelForLoops) // stop searching if all possible loops have been found
            break;
    }
    if(ForLoopsFound < TopLevelForLoops)
        std::cout << "Some for loops deeper than max_depth have not been analyzed" << std::endl;
    analyzeDepth(matches, Data);
}
//step 2: compute stats
void LoopDepthAnalysis::analyzeDepth(Matches<clang::Stmt> matches,
    std::vector<Matches<clang::Stmt>> Data){
        unsigned TopLevelForLoops = matches.size();
        unsigned depth = 1;
        for (auto matches : Data){
            unsigned d = matches.size();
            if (d!=0){
                std::cout << d << "/" << TopLevelForLoops << " of depth "
                << depth << " @ lines: ";
                for (auto m : matches){
                    std::cout << m.location << " ";
                }
                std::cout << std::endl;
                depth++;
            }
        }
}

void LoopDepthAnalysis::analyze(){

}

//step 3: visualization (for later)
// combine
void LoopDepthAnalysis::run(llvm::StringRef InFile, clang::ASTContext& Context
    ){
        std::cout << "\033[32mRunning loop depth analysis:\033[0m" << std::endl;
        MaxDepth=4;
        this->InFile = InFile;
        extract(Context);
}

//-----------------------------------------------------------------------------
