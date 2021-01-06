#include <iostream>

#include "cxx-langstat/Analyses/MoveSemanticsAnalysis.h"

#include "cxx-langstat/Utils.h"

using namespace clang;
using namespace clang::ast_matchers;
using ordered_json = nlohmann::ordered_json;

//-----------------------------------------------------------------------------
// Copied from clang/lib/Sema/SemaTemplateDeduction.cpp or
// https://clang.llvm.org/doxygen/SemaTemplateDeduction_8cpp_source.html#l01190
// For our purposes FirstInnerIndex = 0 is fine.
static bool isForwardingReference(QualType Param, unsigned FirstInnerIndex) {
    // C++1z [temp.deduct.call]p3:
    //   A forwarding reference is an rvalue reference to a cv-unqualified
    //   template parameter that does not represent a template parameter of a
    //   class template.
    if (auto *ParamRef = Param->getAs<RValueReferenceType>()) {
        if (ParamRef->getPointeeType().getQualifiers())
            return false;
        auto *TypeParm = ParamRef->getPointeeType()->getAs<TemplateTypeParmType>();
        return TypeParm && TypeParm->getIndex() >= FirstInnerIndex;
   }
   return false;
 }

//-----------------------------------------------------------------------------

void MoveSemanticsAnalysis::extract(){
    // Writing the matcher like this means that a functionDecl can only bind to
    // one id - good, since it inhibits counting a decl twice under different ids
    DeclarationMatcher functionMatcher = decl(isExpansionInMainFile(), unless(hasParent(functionTemplateDecl())), anyOf(
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


    // DeclarationMatcher parmMatcher = decl(isExpansionInMainFile(), anyOf(
    //     parmVarDecl(unless(hasType(referenceType())))
    //     .bind("value"),
    //     parmVarDecl(hasType(lValueReferenceType(
    //             pointee(unless(isConstQualified())))))
    //     .bind("nonconstlvalueref"),
    //     parmVarDecl(hasType(lValueReferenceType(
    //             pointee(isConstQualified()))))
    //     .bind("constlvalueref"),
    //     parmVarDecl(hasType(rValueReferenceType()))
    //     .bind("rvalueref")
    // ));
    // auto parms = Extractor.extract2(*Context, parmMatcher);
    // CopyParmDecls = getASTNodes<ParmVarDecl>(parms, "value");
    // lValueRefParmDecls = getASTNodes<ParmVarDecl>(parms, "nonconstlvalueref");
    // ConstlValueRefParmDecls = getASTNodes<ParmVarDecl>(parms, "constlvalueref");
    // rValueRefParmDecls = getASTNodes<ParmVarDecl>(parms, "rvalueref");

    /*
    // DeclarationMatcher d = functionTemplateDecl(isExpansionInMainFile(),
    //     has(templateTypeParmDecl().bind("typename")),
    //     has(functionDecl(hasDescendant(
    //         parmVarDecl().bind("parm")
    // ))));
    // auto h = Extractor.extract2(*Context, d);
    // auto typeparms = getASTNodes<TemplateTypeParmDecl>(h, "typename");
    // auto params = getASTNodes<ParmVarDecl>(h, "parm");
    // printMatches("", typeparms);
    // printMatches("", params);
    // for (auto match : params){
    //     // getOriginalType gives QualType, is a ParmVarDecl public method
    //     auto t = match.node->getOriginalType();
    //     std::cout << t.getAsString() << std::endl;
    //     std::cout << t.hasQualifiers() << std::endl;
    //     std::cout << t.getTypePtr()->isRValueReferenceType() << std::endl;
    //     std::cout << isForwardingReference(t, 0) << std::endl;
    //     std::cout << "----" << std::endl;
    // }*/

    // auto isintemplate = hasAncestor(functionDecl(hasParent(functionTemplateDecl().bind("template"))));
    // auto isintemplate = anyOf(unless(hasAncestor(functionDecl(hasParent(functionTemplateDecl().bind("template"))))),
    // hasAncestor(functionDecl().bind("func")));
    // auto isintemplate = hasAncestor(functionDecl().bind("func"));

    auto foft = [](std::string f, std::string ft){
        return anyOf(
            hasAncestor(functionDecl().bind(f)),
            hasAncestor(functionDecl(hasParent(functionTemplateDecl().bind(ft)))));
    };

    auto parmsintemplatesmatcher =  decl(isExpansionInMainFile(), anyOf(
        parmVarDecl(unless(hasType(referenceType())))
        .bind("value"),
        parmVarDecl(hasType(lValueReferenceType(
                pointee(unless(isConstQualified())))))
        .bind("nonconstlvalueref"),
        parmVarDecl(hasType(lValueReferenceType(
                pointee(isConstQualified()))))
        .bind("constlvalueref"),
        parmVarDecl(hasType(rValueReferenceType()))
        .bind("rvalueoruniversalref")
    ));
    auto results = Extractor.extract2(*Context, parmsintemplatesmatcher);
    CopyParmDecls = getASTNodes<ParmVarDecl>(results, "value");
    lValueRefParmDecls = getASTNodes<ParmVarDecl>(results, "nonconstlvalueref");
    ConstlValueRefParmDecls = getASTNodes<ParmVarDecl>(results, "constlvalueref");
    auto rvalueoruniversalref = getASTNodes<ParmVarDecl>(results, "rvalueoruniversalref");
    for(auto match : rvalueoruniversalref){
        auto t = match.node->getOriginalType();
        if(isForwardingReference(t, 0)){
            UniversalRefParmDecls.emplace_back(match);
        } else {
            rValueRefParmDecls.emplace_back(match);
        }
    }

    DeclarationMatcher functionTemplateMatcher = decl(isExpansionInMainFile(), anyOf(
        functionTemplateDecl(has(functionDecl(hasAnyParameter(unless(hasType(referenceType()))))))
        .bind("value"),
        functionTemplateDecl(has(functionDecl(hasAnyParameter(hasType(lValueReferenceType(
                pointee(unless(isConstQualified()))))))))
        .bind("nonconstlvalueref"),
        functionTemplateDecl(has(functionDecl(hasAnyParameter(hasType(lValueReferenceType(
                pointee(isConstQualified())))))))
        .bind("constlvalueref"),
        functionTemplateDecl(has(functionDecl(hasAnyParameter(hasType(rValueReferenceType())))))
        .bind("rvalueref")
    ));

    auto functiontemplates = Extractor.extract2(*Context, functionTemplateMatcher);
    FunctionTemplatesDeclsWithCopy = getASTNodes<FunctionTemplateDecl>(functiontemplates,
        "value");
    FunctionTemplatesDeclsWithlValueRef = getASTNodes<FunctionTemplateDecl>(functiontemplates,
        "nonconstlvalueref");
    FunctionTemplatesDeclsWithConstlValueRef = getASTNodes<FunctionTemplateDecl>(functiontemplates,
        "constlvalueref");
    FunctionTemplatesDeclsWithrValueRef = getASTNodes<FunctionTemplateDecl>(functiontemplates,
        "rvalueref");
}

// parmVarDecl(isExpansionInMainFile(), allOf(anyOf(unless(hasAncestor(functionDecl(hasParent(functionTemplateDecl())))), hasAncestor(functionDecl().bind("f"))), hasAncestor(functionDecl(hasParent(functionTemplateDecl().bind("ft"))))))

// parmVarDecl(isExpansionInMainFile(), anyOf(hasAncestor(functionDecl(hasParent(functionTemplateDecl().bind("ft")))), hasAncestor(functionDecl().bind("f"))))

template<typename T>
void MoveSemanticsAnalysis::gatherData(std::string DeclKind, std::string PassKind,
    const Matches<T>& Matches){
        ordered_json Decls;
        for(auto match : Matches){
            ordered_json d;
            d["location"] = match.location;
            Decls[PassKind][getMatchDeclName(match)].emplace_back(d);
        }
        Result[DeclKind].emplace_back(Decls);
}

void MoveSemanticsAnalysis::run(llvm::StringRef InFile,
    clang::ASTContext& Context) {
        std::cout << "\033[32mRunning MSA:\033[0m" << std::endl;
        this->Context = &Context;
        extract();
        gatherData("function decls", "pass by value", FunctionDeclsWithCopy);
        gatherData("function decls", "pass by non-const lvalue ref", FunctionDeclsWithlValueRef);
        gatherData("function decls", "pass by const lvalue ref", FunctionDeclsWithConstlValueRef);
        gatherData("function decls", "pass by rvalue ref", FunctionDeclsWithrValueRef);

        gatherData("parm decls", "value", CopyParmDecls);
        gatherData("parm decls", "non-const lvalue ref", lValueRefParmDecls);
        gatherData("parm decls", "const lvalue ref", ConstlValueRefParmDecls);
        gatherData("parm decls", "rvalue ref", rValueRefParmDecls);
        gatherData("parm decls", "universal ref", UniversalRefParmDecls);

        gatherData("function template decls", "pass by value", FunctionTemplatesDeclsWithCopy);
        gatherData("function template decls", "pass by non-const lvalue ref", FunctionTemplatesDeclsWithlValueRef);
        gatherData("function template decls", "pass by const lvalue ref", FunctionTemplatesDeclsWithConstlValueRef);
        gatherData("function template decls", "pass by rvalue ref", FunctionTemplatesDeclsWithrValueRef);
        gatherData("function template decls", "pass by universal ref", FunctionTemplatesDeclsWithUniversalRef);







        // for(auto match : FunctionTemplatesDeclsWithrValueRef){
        //     std::cout << match.node->getType().getAsString() << std::endl;
        //     std::cout << match.node->getType().getTypePtr()->getTypeClassName() << std::endl;
        // }
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
