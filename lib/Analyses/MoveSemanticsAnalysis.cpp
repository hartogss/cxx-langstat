#include <iostream>

#include "cxx-langstat/Analyses/MoveSemanticsAnalysis.h"

#include "cxx-langstat/Utils.h"

using namespace clang;
using namespace clang::ast_matchers;
using ordered_json = nlohmann::ordered_json;

//-----------------------------------------------------------------------------

void MoveSemanticsAnalysis::extract(){
    // Writing the matcher like this means that a functionDecl can only bind to
    // one id - good, since it inhibits counting a decl twice under different ids
    DeclarationMatcher functionMatcher = decl(isExpansionInMainFile(), anyOf(
        functionDecl(hasAnyParameter(unless(hasType(referenceType()))))
        .bind("value"), // problem: will also bind to func thats don't have any parameters
        functionDecl(hasAnyParameter(hasType(lValueReferenceType(
                pointee(unless(isConstQualified()))))))
        .bind("nonconstlvalueref"),
        functionDecl(hasAnyParameter(hasType(lValueReferenceType(
                pointee(isConstQualified())))))
        .bind("constlvalueref"),
        functionDecl(hasAnyParameter(hasType(rValueReferenceType())))
        .bind("rvalueref")
    ));
    auto functions = Extractor.extract2(*Context, functionMatcher);
    FunctionDeclsWithCopy = getASTNodes<FunctionDecl>(functions,
        "value");
    FunctionDeclsWithlValueRef = getASTNodes<FunctionDecl>(functions,
        "nonconstlvalueref");
    FunctionDeclsWithConstlValueRef = getASTNodes<FunctionDecl>(functions,
        "constlvalueref");
    FunctionDeclsWithrValueRef = getASTNodes<FunctionDecl>(functions,
        "rvalueref");

    DeclarationMatcher parmMatcher = decl(isExpansionInMainFile(), anyOf(
        parmVarDecl(unless(hasType(referenceType())))
        .bind("value"),
        parmVarDecl(hasType(lValueReferenceType(
                pointee(unless(isConstQualified())))))
        .bind("nonconstlvalueref"),
        parmVarDecl(hasType(lValueReferenceType(
                pointee(isConstQualified()))))
        .bind("constlvalueref"),
        parmVarDecl(hasType(rValueReferenceType()))
        .bind("rvalueref")
    ));
    auto parms = Extractor.extract2(*Context, parmMatcher);
    CopyParmDecls = getASTNodes<ParmVarDecl>(parms, "value");
    lValueRefParmDecls = getASTNodes<ParmVarDecl>(parms, "nonconstlvalueref");
    ConstlValueRefParmDecls = getASTNodes<ParmVarDecl>(parms, "constlvalueref");
    rValueRefParmDecls = getASTNodes<ParmVarDecl>(parms, "rvalueref");


    DeclarationMatcher helper = decl(anyOf( // ameliorate with isExpansionInMainFile
        functionDecl(hasAnyParameter(unless(hasType(referenceType()))))
        .bind("value"), // problem: will also bind to func thats don't have any parameters
        functionDecl(hasAnyParameter(hasType(lValueReferenceType(
                pointee(unless(isConstQualified()))))))
        .bind("nonconstlvalueref"),
        functionDecl(hasAnyParameter(hasType(lValueReferenceType(
                pointee(isConstQualified())))))
        .bind("constlvalueref"),
        functionDecl(hasAnyParameter(hasType(rValueReferenceType())))
        .bind("rvalueref")
    ));

    DeclarationMatcher functionTemplateMatcher = functionTemplateDecl(
        isExpansionInMainFile(),
        has(helper
    ));
    auto functiontemplates = Extractor.extract2(*Context, functionTemplateMatcher);
    FunctionTemplatesDeclsWithCopy = getASTNodes<FunctionDecl>(functiontemplates,
        "value");
    FunctionTemplatesDeclsWithlValueRef = getASTNodes<FunctionDecl>(functiontemplates,
        "nonconstlvalueref");
    FunctionTemplatesDeclsWithConstlValueRef = getASTNodes<FunctionDecl>(functiontemplates,
        "constlvalueref");
    FunctionTemplatesDeclsWithrValueRef = getASTNodes<FunctionDecl>(functiontemplates,
        "rvalueref");
}

void MoveSemanticsAnalysis::gatherStatistics(std::string PassKind,
    const Matches<ParmVarDecl>& Matches){
        ordered_json Decls;
        for(auto match : Matches){
            ordered_json d;
            d["location"] = match.location;
            Decls[PassKind][getMatchDeclName(match)].emplace_back(d);
        }
        Result["parm decls"].emplace_back(Decls);
}

void MoveSemanticsAnalysis::gatherFunctionDeclData(std::string PassKind,
    const Matches<FunctionDecl>& Matches){
        ordered_json Decls;
        for(auto match : Matches){
            ordered_json d;
            d["location"] = match.location;
            Decls[PassKind][getMatchDeclName(match)].emplace_back(d);
        }
        Result["function decls"].emplace_back(Decls);
}

void MoveSemanticsAnalysis::run(llvm::StringRef InFile,
    clang::ASTContext& Context) {
        std::cout << "\033[32mRunning MSA:\033[0m" << std::endl;
        this->Context = &Context;
        extract();
        gatherFunctionDeclData("pass by value", FunctionDeclsWithCopy);
        gatherFunctionDeclData("pass by non-const lvalue ref", FunctionDeclsWithlValueRef);
        gatherFunctionDeclData("pass by const lvalue ref", FunctionDeclsWithConstlValueRef);
        gatherFunctionDeclData("pass by rvalue ref", FunctionDeclsWithrValueRef);
        //
        gatherStatistics("value", CopyParmDecls);
        gatherStatistics("non-const lvalue ref", lValueRefParmDecls);
        gatherStatistics("const lvalue ref", ConstlValueRefParmDecls);
        gatherStatistics("rvalue ref", rValueRefParmDecls);

        // gatherFunctionDeclData("pass by value", FunctionTemplatesDeclsWithCopy);
        // gatherFunctionDeclData("pass by non-const lvalue ref", FunctionTemplatesDeclsWithlValueRef);
        // gatherFunctionDeclData("pass by const lvalue ref", FunctionTemplatesDeclsWithConstlValueRef);
        // gatherFunctionDeclData("pass by rvalue ref", FunctionTemplatesDeclsWithrValueRef);

        for(auto match : FunctionTemplatesDeclsWithrValueRef)
            std::cout << match.node->getType().getAsString() << std::endl;


        // std::cout << Result["rvalue ref parms"]["c"][0].dump(4) << std::endl;
        // // how to avoid indexing using decl name?
        // std::cout << Result["rvalue ref parms"].dump(4) << std::endl;
        // // can also index using iterators:
        // for(auto entriesForASingleVarName : Result["rvalue ref parms"]){
        //     std::cout << varName << std::endl;
        //     // for(auto loc : varName){
        //         // std::cout << varName << "@ " << loc << std::endl;
        //     // }
        // }


}

//-----------------------------------------------------------------------------
