#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

#include <iostream>

#include "cxx-langstat/Extraction.h"

// namespaces
using namespace clang; // CompilerInstance, ASTFrontendAction, ASTConsumer
using namespace clang::ast_matchers; // StatementMatcher, actual AST matchers like forStmt etc.
using namespace clang::tooling; // CommonOptionsParser

//-----------------------------------------------------------------------------

Match::Match(unsigned location, const void* node, ASTContext* ctxt) : location(location), node(node), ctxt(ctxt){
    // std::cout<<"Match ctor"<<std::endl;
}
Match::~Match(){
    // std::cout<<"Match dtor"<<std::endl;
}

//-----------------------------------------------------------------------------

// ctor
Extractor::Extractor(clang::tooling::ClangTool Tool) : Tool(Tool){
    std::cout<<"Extractor ctor"<<std::endl;
}
Extractor::~Extractor(){
    std::cout<<"Extractor dtor"<<std::endl;
}
void Extractor::run(const MatchFinder::MatchResult &Result) {
    // maybe need template or switch statement? could also put 'kind' of node into id to trigger right code
    // one could do this structure of if statement to check what it is
    // one would however go from more to less specific to see what it is
    ASTContext* Context = Result.Context;
     // I think this can also match call expr, since expr is a special kind of stmt,
    if(const Stmt* node = Result.Nodes.getNodeAs<Stmt>(this->matcherid)){
        unsigned Location = Context->getFullLoc(node->getBeginLoc()).getLineNumber();
        const char* StmtKind = node->getStmtClassName();
        if(StmtKind == "CallExpr"){
            CallExpr* n = (CallExpr*)node;
            Match m(Location, n, Context);
            matches.emplace_back(m);
            std::string calleeName = n->getDirectCallee()->getNameInfo().getAsString();
        } else if(StmtKind == "ForStmt"){
            ForStmt* n = (ForStmt*)node;
            Match m(Location, n, Context);
            matches.emplace_back(m);
        } else if(StmtKind == "WhileStmt"){
            WhileStmt* n = (WhileStmt*)node;
            Match m(Location, n, Context);
            matches.emplace_back(m);
        } else if(StmtKind == "DoStmt"){
            DoStmt* n = (DoStmt*)node;
            Match m(Location, n, Context);
            matches.emplace_back(m);
        } else {
            std::cout << "\033[31m->inexplicable stmt\033[0m" << std::endl;
        }
    } else if(const Decl* node = Result.Nodes.getNodeAs<Decl>(this->matcherid)){
        unsigned Location = Context->getFullLoc(node->getBeginLoc()).getLineNumber();
        if(const FunctionDecl* node = Result.Nodes.getNodeAs<FunctionDecl>(this->matcherid)){
            Match m(Location, node, Context);
            matches.emplace_back(m);
        } else {
            std::cout << "\033[31m->inexplicable decl\033[0m" << std::endl;
        }
    } else {
        std::cout << "\033[31m->inexplicable node\033[0m" << std::endl;
    }
}
void Extractor::resetState(){
    matches.clear(); // does this ensure no memory leak?
}
Matches Extractor::extract(std::string id, StatementMatcher Matcher){
    resetState();
    MatchFinder Finder;
    this->matcherid=id;
    Finder.addMatcher(Matcher, this);
    Tool.run(newFrontendActionFactory(&Finder).get());
    return matches; //according to my understanding this returns a copy to caller, how is that copy constructed at the caller?
}
Matches Extractor::extract(std::string id, DeclarationMatcher Matcher){
    resetState();
    MatchFinder Finder;
    this->matcherid=id;
    Finder.addMatcher(Matcher, this);
    Tool.run(newFrontendActionFactory(&Finder).get());
    return matches; //according to my understanding this returns a copy to caller, how is that copy constructed at the caller?
}

//-----------------------------------------------------------------------------
