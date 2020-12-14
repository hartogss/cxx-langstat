#include <iostream>
#include <vector>

#include "cxx-langstat/Analyses/VariableTemplateAnalysis.h"
#include "cxx-langstat/Utils.h"

using namespace clang;
using namespace clang::ast_matchers;
using ordered_json = nlohmann::ordered_json;

//-----------------------------------------------------------------------------
// Question: did programmers abandon classes with static fields and constexpr
// functions returning the value in favor of variable templates, which were
// introduced in C++14?

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

    ClassWithStaticMemberDecls = Extractor.extract(*Context,
        "classwithstaticmember", ClassWithStaticMember);
    ConstexprFunctionDecls = Extractor.extract(*Context, "constexprfunction",
        ConstexprFunction);
    VariableTemplateDecls = Extractor.extract(*Context, "variabletemplate",
        VariableTemplate);
}
void VariableTemplateAnalysis::gatherStatistics(){
    // Possible improvment: report if class template contains multiple static
    // member vars
    ordered_json CTSMs;
    for(auto match : ClassWithStaticMemberDecls){
        ordered_json CTSM;
        CTSM["location"] = match.location;
        CTSMs[getMatchDeclName(match)] = CTSM;
    }
    ordered_json CTFs;
    for(auto match : ConstexprFunctionDecls){
        ordered_json CTF;
        CTF["location"] = match.location;
        CTFs[getMatchDeclName(match)] = CTF;
    }
    ordered_json VTs;
    for(auto match : VariableTemplateDecls){
        ordered_json VT;
        VT["location"] = match.location;
        VTs[getMatchDeclName(match)] = VT;
    }
    Result["class templates with static member"] = CTSMs;
    Result["constexpr function templates"] = CTFs;
    Result["variable templates"] = VTs;
}
void VariableTemplateAnalysis::run(llvm::StringRef InFile,
    clang::ASTContext& Context){
        std::cout << "\033[32mRunning variable template analysis:\033[0m\n";
        this->Context = &Context;
        extract();
        gatherStatistics();
}

//-----------------------------------------------------------------------------
