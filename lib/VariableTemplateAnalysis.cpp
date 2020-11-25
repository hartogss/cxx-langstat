#include <iostream>
#include <vector>

#include "cxx-langstat/VariableTemplateAnalysis.h"

using namespace clang;
using namespace clang::ast_matchers;

//-----------------------------------------------------------------------------

std::string getDeclName(Match<clang::Decl> node){
    if(auto n = dyn_cast<clang::NamedDecl>(node.node)){
        return n->getNameAsString();
    } else {
        std::cout << "Decl @ " << node.location << "cannot be resolved" << std::endl;
        return "INVALID";
    }
}

void printStatistics(std::string text, Matches<clang::Decl> matches){
    std::cout << "\033[33m" << text << ":\033[0m " << matches.size() << "\n";
    for(auto m : matches)
        std::cout << getDeclName(m) << " @ " << m.location << std::endl;
}

//-----------------------------------------------------------------------------
// Question: did programmers abandon classes with static fields and constexpr
// functions returning the value in favor of variable templates, which were
// introduced in C++14?

VariableTemplateAnalysis::VariableTemplateAnalysis(ASTContext& Context) :
    Analysis(Context) {
}

// clang::ast_matchers::AST_MATCHER(VarTemplateDecl, has){
//
// }

void VariableTemplateAnalysis::extract(){

    DeclarationMatcher ClassWithStaticVar =
    cxxRecordDecl(has(varDecl(isStaticStorageClass())))
    .bind("classwithstaticvar");

    DeclarationMatcher ConstexprFunction = functionTemplateDecl(
        isExpansionInMainFile(),
        // FunctionDecl that is Constexpr
        has(functionDecl(
            isConstexpr(),
            // whose body contains
            has(compoundStmt(
                // at least a variable declaration and a return that returns it
                has(declStmt(has(varDecl().bind("datadecl")))),
                has(returnStmt(has(
                    declRefExpr(to(varDecl(equalsBoundNode("datadecl"))))))),
                // and does not contain anything else. Sufficient to filter for
                // statement since function body contains statements only. Decl
                // inside of body handled via a declStmt - which is a Stmt too.
                unless(has(stmt(
                    unless(anyOf(
                        declStmt(has(varDecl())),
                        returnStmt()))))))))))
    .bind("constexprfunction");

    // "Write" matcher for variable templates that didn't exist yet:
    // http://clang.llvm.org/docs/LibASTMatchers.html#writing-your-own-matchers
    // https://clang.llvm.org/doxygen/ASTMatchersInternal_8cpp_source.html
    internal::VariadicDynCastAllOfMatcher<Decl, VarTemplateDecl> varTemplateDecl;
    DeclarationMatcher VariableTemplate = varTemplateDecl().bind("variabletemplate");

    auto ClassWithStaticVarDecls = Extr.extract("classwithstaticvar", ClassWithStaticVar);
    auto ConstexprFunctionDecls = Extr.extract("constexprfunction", ConstexprFunction);
    auto VariableTemplateDecls = Extr.extract("variabletemplate", VariableTemplate);

    printStatistics("Class templates with static data", ClassWithStaticVarDecls);
    printStatistics("Constexpr function templates", ConstexprFunctionDecls);
    printStatistics("Variable templates", VariableTemplateDecls);
}
void VariableTemplateAnalysis::analyze(){
}
void VariableTemplateAnalysis::run(){
    extract();
    analyze();
}

//-----------------------------------------------------------------------------
