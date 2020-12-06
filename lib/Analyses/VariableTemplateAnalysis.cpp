#include <iostream>
#include <vector>

#include "cxx-langstat/Analyses/VariableTemplateAnalysis.h"
#include "cxx-langstat/Utils.h"

using namespace clang;
using namespace clang::ast_matchers;

//-----------------------------------------------------------------------------
// Question: did programmers abandon classes with static fields and constexpr
// functions returning the value in favor of variable templates, which were
// introduced in C++14?

VariableTemplateAnalysis::VariableTemplateAnalysis(llvm::StringRef InFile,
    ASTContext& Context) :
        Analysis(InFile, Context) {
}
void VariableTemplateAnalysis::extract(){
    // First pre-C++14 idiom to get variable template functionality:
    // Class templates with static data.
    // Seems like static members of classes in AST are vars, not fields.
    // Essentially the same matcher as for typedefs in UsingAnalysis.cpp,
    // but static vardecl instead of typedefs.
    DeclarationMatcher ClassWithStaticMember = classTemplateDecl(
        isExpansionInMainFile(),
        has(cxxRecordDecl(
            // Must have static member
            forEach(varDecl(isStaticStorageClass()).bind("staticmember")),
            // Mustn't have decl that is not static member, access spec or cxxrecord
            unless(has(decl(
                unless(anyOf(
                    varDecl(isStaticStorageClass()),
                    accessSpecDecl(),
                    cxxRecordDecl()))))))))
    .bind("classwithstaticmember");

    // Second pre-C++14 idiom:
    DeclarationMatcher ConstexprFunction = functionTemplateDecl(
        isExpansionInMainFile(),
        // FunctionDecl that is Constexpr
        has(functionDecl(
            isConstexpr(),
            // whose body contains
            has(compoundStmt(
                // at least a variable declaration and a return that returns it
                forEach(declStmt(has(varDecl().bind("datadecl")))),
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

    // C++14 variable templates
    // "Write" matcher for variable templates that didn't exist yet:
    // http://clang.llvm.org/docs/LibASTMatchers.html#writing-your-own-matchers
    // https://clang.llvm.org/doxygen/ASTMatchersInternal_8cpp_source.html
    internal::VariadicDynCastAllOfMatcher<Decl, VarTemplateDecl> varTemplateDecl;
    DeclarationMatcher VariableTemplate = varTemplateDecl(
        isExpansionInMainFile())
    .bind("variabletemplate");

    ClassWithStaticMemberDecls = Extr.extract("classwithstaticmember", ClassWithStaticMember);
    ConstexprFunctionDecls = Extr.extract("constexprfunction", ConstexprFunction);
    VariableTemplateDecls = Extr.extract("variabletemplate", VariableTemplate);
}
void VariableTemplateAnalysis::analyze(){
    printMatches("Class templates with static member", ClassWithStaticMemberDecls);
    printMatches("Constexpr function templates", ConstexprFunctionDecls);
    printMatches("Variable templates", VariableTemplateDecls);
}
void VariableTemplateAnalysis::run(){
    std::cout << "\033[32mRunning variable template analysis:\033[0m\n";
    extract();
    analyze();
}

//-----------------------------------------------------------------------------
