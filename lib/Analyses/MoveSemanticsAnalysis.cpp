#include <iostream>

#include "cxx-langstat/Analyses/MoveSemanticsAnalysis.h"

#include "cxx-langstat/Utils.h"

using namespace clang;
using namespace clang::ast_matchers;
using ordered_json = nlohmann::ordered_json;

//-----------------------------------------------------------------------------
/// Get the index of the first template parameter that was originally from the
/// innermost template-parameter-list. This is 0 except when we concatenate
/// the template parameter lists of a class template and a constructor template
/// when forming an implicit deduction guide.
static unsigned getFirstInnerIndex(FunctionTemplateDecl *FTD) {
  auto *Guide = dyn_cast<CXXDeductionGuideDecl>(FTD->getTemplatedDecl());
  if (!Guide || !Guide->isImplicit())
 return 0;
  return Guide->getDeducedTemplate()->getTemplateParameters()->size();
}
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

    internal::VariadicDynCastAllOfMatcher<Type, PackExpansionType> packExpansionType;

    // Writing the matcher like this means that a functionDecl can only bind to
    // one id - good, since it inhibits counting a decl twice under different ids

    // Extract functions that use each kind of parameter: by value, non-const
    // lvalue ref, const lvalue ref and rvalue ref
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
    FuncsWithValueParm = getASTNodes<FunctionDecl>(functions,
        "value");
    FuncsWithNonConstLValueRefParm = getASTNodes<FunctionDecl>(functions,
        "nonconstlvalueref");
    FuncsWithConstLValueRefParm = getASTNodes<FunctionDecl>(functions,
        "constlvalueref");
    FuncsWithRValueRefParm = getASTNodes<FunctionDecl>(functions,
        "rvalueref");

    // Same as above, but for function templates; additionally look at which
    // templates use universal/forwarding references
    DeclarationMatcher functionTemplateMatcher = decl(isExpansionInMainFile(), anyOf(
        functionTemplateDecl(has(functionDecl(hasAnyParameter(unless(anyOf(
            hasType(referenceType()),
            hasType(packExpansionType())))))))
        .bind("value"),
        functionTemplateDecl(has(functionDecl(hasAnyParameter(hasType(lValueReferenceType(
                pointee(unless(isConstQualified()))))))))
        .bind("nonconstlvalueref"),
        functionTemplateDecl(has(functionDecl(hasAnyParameter(hasType(lValueReferenceType(
                pointee(isConstQualified())))))))
        .bind("constlvalueref"),
        functionTemplateDecl(has(functionDecl(hasAnyParameter(hasType(rValueReferenceType())))))
        .bind("rvalueoruniversalref"),
        functionTemplateDecl(has(functionDecl(hasAnyParameter(hasType(packExpansionType())))))
        .bind("pack")
    ));
    auto functiontemplates = Extractor.extract2(*Context, functionTemplateMatcher);
    FuncTemplatesWithValueParm = getASTNodes<FunctionTemplateDecl>(functiontemplates,
        "value");
    FuncTemplatesWithNonConstLValueRefParm = getASTNodes<FunctionTemplateDecl>(functiontemplates,
        "nonconstlvalueref");
    FuncTemplatesWithConstLValueRefParm = getASTNodes<FunctionTemplateDecl>(functiontemplates,
        "constlvalueref");
    auto FTsWithRValueOrUniversalRefParm = getASTNodes<FunctionTemplateDecl>(functiontemplates,
        "rvalueoruniversalref");
    auto FTsWithPackParm = getASTNodes<FunctionTemplateDecl>(functiontemplates,
        "pack");

    // Extract the parameters itself from the function and function template decls
    // above.
    // Room for improvement: when extracting parameters, also extract the function
    // (template) they're contained in, s.t. the matchers above become unnecessary
    // leading to smaller, hopefully more maintainable and intuitive code.
    // This matcher would become more complicated tho.
    // function or function template matcher to parallel to parameters extract
    // function (templates).
    // auto foft = [](std::string f, std::string ft){
    //     return anyOf(
    //         hasAncestor(functionDecl().bind(f)),
    //         hasAncestor(functionDecl(hasParent(functionTemplateDecl().bind(ft)))));
    // };
    auto parmsintemplatesmatcher =  decl(isExpansionInMainFile(), anyOf(
        parmVarDecl(unless(anyOf(
            hasType(referenceType()),
            hasType(packExpansionType()))))
        .bind("value"),
        parmVarDecl(hasType(lValueReferenceType(
                pointee(unless(isConstQualified())))))
        .bind("nonconstlvalueref"),
        parmVarDecl(hasType(lValueReferenceType(
                pointee(isConstQualified()))))
        .bind("constlvalueref"),
        parmVarDecl(hasType(rValueReferenceType()))
        .bind("rvalueoruniversalref"),
        parmVarDecl(hasType(packExpansionType()))
        .bind("pack")
    ));
    auto parms = Extractor.extract2(*Context, parmsintemplatesmatcher);
    ValueParms = getASTNodes<ParmVarDecl>(parms, "value");
    NonConstLValueRefParms = getASTNodes<ParmVarDecl>(parms, "nonconstlvalueref");
    ConstLValueRefParms = getASTNodes<ParmVarDecl>(parms, "constlvalueref");
    auto RValueOrUniversalRefParms = getASTNodes<ParmVarDecl>(parms, "rvalueoruniversalref");
    auto PackParms = getASTNodes<ParmVarDecl>(parms, "pack");
    for(unsigned idx=0; idx<PackParms.size(); idx++){
        auto match = PackParms[idx];
        // Qualified type
        auto ot = match.node->getOriginalType(); // should be pack
        std::cout << ot.getTypePtr()->getTypeClassName() << std::endl;
        std::cout << ot.getAsString() << std::endl;
        // Can use 'cast' here, have good reason to believe that is PackExpType
        // Gives us type that is being 'packed', which can be reference or
        // something else
        auto qualpatterntype = cast<PackExpansionType>(ot.getTypePtr())->getPattern();
        std::cout << qualpatterntype.getTypePtr()->getTypeClassName() << std::endl;
        std::cout << qualpatterntype.getAsString() << std::endl;
        std::cout << "----" << std::endl;
        // Gives us type that is being 'packed', but without qualifiers
        auto type = qualpatterntype.getTypePtr();
        if(type->isLValueReferenceType()){
            if(type->getPointeeType().isConstQualified()) {
                ConstLValueRefParms.emplace_back(match);
                FuncTemplatesWithConstLValueRefParm.emplace_back(FTsWithPackParm[idx]);
            } else {
                NonConstLValueRefParms.emplace_back(match);
                FuncTemplatesWithNonConstLValueRefParm.emplace_back(FTsWithPackParm[idx]);
            }
        } else if(type->isRValueReferenceType()){
            RValueOrUniversalRefParms.emplace_back(match);
            FTsWithRValueOrUniversalRefParm.emplace_back(FTsWithPackParm[idx]);
        } else {
            ValueParms.emplace_back(match);
            FuncTemplatesWithValueParm.emplace_back(FTsWithPackParm[idx]);
        }
    }
    std::cout << "#----" << std::endl;

    printMatches("", FTsWithRValueOrUniversalRefParm);
    printMatches(" ", RValueOrUniversalRefParms);

    // For function templates with rvalue or universal references, then split those
    // into two separate categories.
    if(FTsWithRValueOrUniversalRefParm.size() != 0){
        for(unsigned idx=0; idx<RValueOrUniversalRefParms.size(); idx++){
            std::cout << "test" << std::endl;
            auto match = RValueOrUniversalRefParms[idx];
            auto t = match.node->getOriginalType(); // gives QualType
            // If is Pack, strip away pack to get type that is being packed to
            // inspect its kind of reference
            if(isa<PackExpansionType>(t.getTypePtr()))
                t = cast<PackExpansionType>(t.getTypePtr())->getPattern();
            if(isForwardingReference(t, 0 )){
                UniversalRefParms.emplace_back(match);
                FuncTemplatesWithUniversalRefParm.emplace_back(FTsWithRValueOrUniversalRefParm[idx]);
            } else {
                RValueRefParms.emplace_back(match);
                FuncTemplatesWithRValueRefParm.emplace_back(FTsWithRValueOrUniversalRefParm[idx]);
            }
        }
    } else { // otherwise, every potential universal reference simply is rvalue ref
        RValueRefParms = RValueOrUniversalRefParms;
    }
}

template<typename T>
void MoveSemanticsAnalysis::gatherData(std::string DeclKind, std::string PassKind,
    const Matches<T>& Matches){
        ordered_json Decls;
        for(auto match : Matches){
            ordered_json d;
            d["location"] = match.location;
            Decls[getMatchDeclName(match)].emplace_back(d);
        }
        Result[DeclKind][PassKind] = Decls;
}

void MoveSemanticsAnalysis::run(llvm::StringRef InFile,
    clang::ASTContext& Context) {
        std::cout << "\033[32mRunning MSA:\033[0m" << std::endl;
        this->Context = &Context;
        extract();

        gatherData("function decls", "pass by value", FuncsWithValueParm);
        gatherData("function decls", "pass by non-const lvalue ref", FuncsWithNonConstLValueRefParm);
        gatherData("function decls", "pass by const lvalue ref", FuncsWithConstLValueRefParm);
        gatherData("function decls", "pass by rvalue ref", FuncsWithRValueRefParm);

        gatherData("parm decls", "value", ValueParms);
        gatherData("parm decls", "non-const lvalue ref", NonConstLValueRefParms);
        gatherData("parm decls", "const lvalue ref", ConstLValueRefParms);
        gatherData("parm decls", "rvalue ref", RValueRefParms);
        gatherData("parm decls", "universal ref", UniversalRefParms);

        gatherData("function template decls", "pass by value", FuncTemplatesWithValueParm);
        gatherData("function template decls", "pass by non-const lvalue ref", FuncTemplatesWithNonConstLValueRefParm);
        gatherData("function template decls", "pass by const lvalue ref", FuncTemplatesWithConstLValueRefParm);
        gatherData("function template decls", "pass by rvalue ref", FuncTemplatesWithRValueRefParm);
        gatherData("function template decls", "pass by universal ref", FuncTemplatesWithUniversalRefParm);



        // Code to test printing JSON objects without knowing index ids
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

        // Old code for trying to figure out if universal reference or not
        /*
        // for(auto match : FunctionTemplatesDeclsWithrValueRef){
        //     std::cout << match.node->getType().getAsString() << std::endl;
        //     std::cout << match.node->getType().getTypePtr()->getTypeClassName() << std::endl;
        // }
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
}

//-----------------------------------------------------------------------------
