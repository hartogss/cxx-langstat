#include <iostream>
#include <vector>

#include "cxx-langstat/VariableTemplateAnalysis.h"

using namespace clang;
using namespace clang::ast_matchers;

//-----------------------------------------------------------------------------
// Question: did programmers abandon classes with static fields and constexpr
// functions returning the value in favor of variable templates, which were
// introduced in C++14?

VariableTemplateAnalysis::VariableTemplateAnalysis(ASTContext& Context) :
    Analysis(Context) {
}
void VariableTemplateAnalysis::extract(){

    DeclarationMatcher VariableTemplate = varDecl(
        has(cxxUnresolvedConstructExpr())
    ).bind("variabletemplate");

    DeclarationMatcher ClassWithStaticVar =
    cxxRecordDecl(has(varDecl(isStaticStorageClass())))
    .bind("classwithstaticvar");

    DeclarationMatcher ConstexprFunction = functionTemplateDecl(
        has(functionDecl(
            isConstexpr(),
            has(varDecl()),
            unless(has(decl(unless(varDecl())))))))
    .bind("constexprfunction");


}
void VariableTemplateAnalysis::analyze(){
}
void VariableTemplateAnalysis::run(){
}

//-----------------------------------------------------------------------------
