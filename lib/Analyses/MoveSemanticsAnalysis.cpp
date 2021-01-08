#include <iostream>

#include "cxx-langstat/Analyses/MoveSemanticsAnalysis.h"

#include "cxx-langstat/Utils.h"

using namespace clang;
using namespace clang::ast_matchers;
using ordered_json = nlohmann::ordered_json;

//-----------------------------------------------------------------------------
// Copied from clang/lib/Sema/SemaTemplateDeduction.cpp

// https://clang.llvm.org/doxygen/SemaTemplateDeduction_8cpp_source.html#l01182
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
        std::cout << "typeparm:  " << TypeParm->getIndex() << std::endl;
        return TypeParm && TypeParm->getIndex() >= FirstInnerIndex;
   }
   return false;
 }

// Should be the same as the function above, but above does not work the way I want
bool isUniversalReference(FunctionTemplateDecl* FTD, QualType Param){
    if(FTD){
        // Given parm should be an rvalue reference
        if(const Type* ParamRef = Param->getAs<RValueReferenceType>()) {
            // Should not have qualifiers
            if(ParamRef->getPointeeType().getQualifiers())
                return false;
            // pointee type of parm should be a type that is defined by a template type parameter
            if(const Type* TypeParm = ParamRef->getPointeeType()->getAs<TemplateTypeParmType>()){ // has type Type*
                auto this1 = TypeParm->getCanonicalTypeUnqualified();
                std::cout << "1\n";
                this1.dump();
                TemplateParameterList* TPL = FTD->getTemplateParameters();
                // for the given function template, check if any of the type
                // parameters could be the type of our possible universal reference
                // is this loop even necessary, shouldn't the above loop be sufficient?
                // I came up with this after reading
                // https://en.cppreference.com/w/cpp/language/reference
                // LIke this, I restrict from which template the type can actually come from
                for(unsigned idx=0; idx<TPL->size(); idx++){
                    NamedDecl* CurParam = TPL->getParam(idx);
                    if(auto TTPDecl = dyn_cast<TemplateTypeParmDecl>(CurParam)){
                        auto this2 = TTPDecl->getTypeForDecl()->getCanonicalTypeUnqualified();
                        this2.dump();
                        if(this1==this2)
                            return true;
                    }
                }
            }
        }
    }
    return false;
}

bool isParameterPackType(QualType Param){
    return isa<PackExpansionType>(Param.getTypePtr());
}

//-----------------------------------------------------------------------------
void MoveSemanticsAnalysis::addFunction(const Match<FunctionTemplateDecl>& match, std::map<std::string, bool> ParmMap){
    if(ParmMap.at("value"))
        FuncTemplatesWithValueParm.emplace_back(match);
    if(ParmMap.at("nonconstlvalueref"))
        FuncTemplatesWithNonConstLValueRefParm.emplace_back(match);
    if(ParmMap.at("constlvalueref"))
        FuncTemplatesWithConstLValueRefParm.emplace_back(match);
    if(ParmMap.at("rvalueref"))
        FuncTemplatesWithRValueRefParm.emplace_back(match);
    if(ParmMap.at("universalref"))
        FuncTemplatesWithUniversalRefParm.emplace_back(match);
}
void MoveSemanticsAnalysis::addFunction(const Match<FunctionDecl>& match, std::map<std::string, bool> ParmMap){
    if(ParmMap.at("value"))
        FuncsWithValueParm.emplace_back(match);
    if(ParmMap.at("nonconstlvalueref"))
        FuncsWithNonConstLValueRefParm.emplace_back(match);
    if(ParmMap.at("constlvalueref"))
        FuncsWithConstLValueRefParm.emplace_back(match);
    if(ParmMap.at("rvalueref"))
        FuncsWithRValueRefParm.emplace_back(match);
}

template<typename T>
void MoveSemanticsAnalysis::associateParameters(const Matches<T>& Matches){
    // For each function (template)
    for(auto match : Matches){

        // print func
        if(auto n = dyn_cast<NamedDecl>(match.node)){
            std::cout << n->getQualifiedNameAsString() << " @ " << match.location << std::endl;
        }

        auto Node = match.node;
        FunctionDecl* Func;
        auto WasTemplate = false;
        FunctionTemplateDecl* FTD = nullptr;
        std::map<std::string, bool> ParmMap = {
            {"value", false}, {"nonconstlvalueref", false},
            {"constlvalueref", false}, {"rvalueref", false},
            {"universalref", false}};
        // If given vector of function templates, look at contained function decls.
        if(auto ft = dyn_cast<FunctionTemplateDecl>(Node)){
            Func = ft->getTemplatedDecl();
            WasTemplate = true;
            FTD = const_cast<FunctionTemplateDecl*>(ft);
        } else if(auto f = dyn_cast<FunctionDecl>(Node)){
            Func = const_cast<FunctionDecl*>(f);
        }
        // For each parameter of the function
        for(auto Param : Func->parameters()){
            // print parm
            if(auto n = dyn_cast<NamedDecl>(Param)){
                std::cout << "param type: " << n->getQualifiedNameAsString() << std::endl;
            }
            // Create Match from parameter that bundles it with location&context
            unsigned Location = Context->getFullLoc(
                Param->getBeginLoc()).getLineNumber();
            Match<ParmVarDecl> ParmMatch(Location, Param, Context);
            // If parameter pack is used, strip it off.
            QualType OriginalType = Param->getOriginalType();
            QualType qt = OriginalType;
            if(isParameterPackType(OriginalType)){
                // Gives us the the type that is 'being packed'.
                QualType PatternType = cast<PackExpansionType>
                    (OriginalType.getTypePtr())->getPattern();
                qt = PatternType;
            }
            const Type* type = qt.getTypePtr();
            // Split into value, lvalue ref and rvalue ref groups.
            if(type->isLValueReferenceType()){
                if(type->getPointeeType().isConstQualified()){
                    ConstLValueRefParms.emplace_back(ParmMatch);
                    ParmMap.at("constlvalueref") = true;
                }else{
                    NonConstLValueRefParms.emplace_back(ParmMatch);
                    ParmMap.at("nonconstlvalueref") = true;
                }
            }else if(type->isRValueReferenceType()){
                if(isUniversalReference(FTD, qt) && WasTemplate){
                    UniversalRefParms.emplace_back(ParmMatch);
                    ParmMap.at("universalref") = true;
                }else{
                    RValueRefParms.emplace_back(ParmMatch);
                    ParmMap.at("rvalueref") = true;
                }
            }else{
                ValueParms.emplace_back(ParmMatch);
                ParmMap.at("value") = true;
            }
        }
        addFunction(match, ParmMap);
        std::cout << "--------" << std::endl;
    }
}

void MoveSemanticsAnalysis::extract(){
    internal::VariadicDynCastAllOfMatcher<Type, PackExpansionType> packExpansionType;

    auto ftmatcher = functionTemplateDecl(isExpansionInMainFile()).bind("ft");
    auto ftresult = Extractor.extract2(*Context, ftmatcher);
    auto fts = getASTNodes<FunctionTemplateDecl>(ftresult, "ft");
    associateParameters(fts);
    // printMatches("fts", fts);

    auto fmatcher = functionDecl(isExpansionInMainFile(), unless(hasParent(functionTemplateDecl()))).bind("f");
    auto fresult = Extractor.extract2(*Context, fmatcher);
    auto fs = getASTNodes<FunctionDecl>(fresult, "f");
    associateParameters(fs);
}
// issues: 1. universal reference cannot be part of function (non-template) decl,
// because the type deduction will not take place at its call site. however, a non-template
// function can be variadic if it is contained e.g. in a class or function template
// 2. isForwardingReference may not always be called with 0 because of same reason
// 3. what about constructor in class templates? analyze those too? or separately probably better, by looking at copy, move constructors separately.

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
