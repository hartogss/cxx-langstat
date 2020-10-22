#include "clang/ASTMatchers/ASTMatchers.h"

#include <iostream>

#include "ForStmtAnalysis.h"

using namespace clang; // Stmt, Expr, FullSourceLoc
using namespace clang::ast_matchers;

// TODO: variables defined here shouldn't be visible in Langstat.cpp, only
// analysis object should be

StatementMatcher constructMatcher(StatementMatcher m, int depth, std::string Name){
    StatementMatcher AtLeastDepth = hasDescendant(forStmt());
    StatementMatcher EnsureIsOuterMostLoop = unless(hasAncestor(forStmt()));
    int i=2;
    while(i<depth){
        AtLeastDepth = hasDescendant(forStmt(AtLeastDepth));
        i++;
    }
    StatementMatcher AtLeastDepthPLUS1 = forStmt(EnsureIsOuterMostLoop, hasDescendant(forStmt(AtLeastDepth)));
    AtLeastDepth = forStmt(EnsureIsOuterMostLoop, AtLeastDepth);
    return forStmt(AtLeastDepth, unless(AtLeastDepthPLUS1)).bind(Name);
}

// matchers with more depth could either be hardcoded
// or made constexpr if max-depth is fixed to speed up runtime
int depth = 3;


std::string Name = "fs";
StatementMatcher BaseMatcher = forStmt();
// StatementMatcher test = forStmt(hasDescendant(BaseMatcher));
StatementMatcher m = constructMatcher(BaseMatcher, depth, Name);

auto Runner = [](const clang::ast_matchers::MatchFinder::MatchResult& Result) {
    const ForStmt* FS = Result.Nodes.getNodeAs<ForStmt>(Name);
    clang::ASTContext* Context = Result.Context;
    /*const Stmt* LoopInit = FS->getInit();
    const Expr* LoopCond = FS->getCond();
    const Expr* LoopInc = FS->getInc();*/
    if(FS){
        FullSourceLoc Location = Context->getFullLoc(FS->getBeginLoc());
        if(Location.isValid()){
            int LineNumber = Location.getLineNumber();
            std::cout << "for loop of depth " << depth << " @ line " << LineNumber << "\n";
            /*LoopInit->dumpPretty(*Context);
            LoopCond->dumpPretty(*Context);
            LoopInc->dumpPretty(*Context);*/
        }
    }
};

ForStmtAnalysis forstmt(Name, m, Runner);
