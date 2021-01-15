#include <iostream>

#include "cxx-langstat/Analyses/MoveSemanticsAnalysis.h"

#include "cxx-langstat/Utils.h"

using namespace clang;
using namespace clang::ast_matchers;
using ordered_json = nlohmann::ordered_json;

//-----------------------------------------------------------------------------

// An rvalue reference Param of a function is a universal/forwarding reference
// if it refers to a cv-unqualified type defined by a template type parameters
// of the function template declaration that belongs to that function (i.e., the
// type parameters might not come from a template class or function encompassing
// it)
// https://en.cppreference.com/w/cpp/language/reference#Forwarding_references
bool isUniversalReference(FunctionTemplateDecl* FTD, QualType Param){
    if(FTD){
        // Given parm should be an rvalue reference
        if(const Type* ParamRef = Param->getAs<RValueReferenceType>()) {
            // Should not have qualifiers
            if(ParamRef->getPointeeType().getQualifiers())
                return false;
            // Pointee type of parm should be a type that is defined by a
            // template type parameter
            if(const Type* TypeParm = ParamRef->getPointeeType()->getAs<TemplateTypeParmType>()){ // has type Type*
                CanQualType PointeeCanonicalType = TypeParm->getCanonicalTypeUnqualified();
                // PointeeCanonicalType.dump();
                TemplateParameterList* TPL = FTD->getTemplateParameters();
                // For the given function template, check if any of the template
                // type parameters could be the type of our possible universal
                // reference.
                // Is this overkill?
                for(unsigned idx=0; idx<TPL->size(); idx++){
                    NamedDecl* CurParam = TPL->getParam(idx);
                    if(auto TTPDecl = dyn_cast<TemplateTypeParmDecl>(CurParam)){
                        CanQualType TemplateTypeParmCanonicalType = TTPDecl
                            ->getTypeForDecl()->getCanonicalTypeUnqualified();
                        // TemplateTypeParmCanonicalType.dump();
                        if(PointeeCanonicalType==TemplateTypeParmCanonicalType)
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

// For each match, figures out the type of the parameters and puts it in the
// appropriate vector.
// T should be clang::FunctionDecl or clang::FunctionTemplateDecl
template<typename T>
void MoveSemanticsAnalysis::associateParameters(const Matches<T>& Matches){
    // For each function (template)
    for(auto match : Matches){
        auto Node = match.node;
        FunctionDecl* Func;
        auto WasTemplate = false;
        FunctionTemplateDecl* FTD = nullptr;
        std::map<std::string, bool> ParmMap = {
            {"value", false}, {"nonconstlvalueref", false},
            {"constlvalueref", false}, {"rvalueref", false},
            {"universalref", false}};
        // If function templates, look at contained function decl.
        if(auto ft = dyn_cast<FunctionTemplateDecl>(Node)){
            Func = ft->getTemplatedDecl();
            WasTemplate = true;
            FTD = const_cast<FunctionTemplateDecl*>(ft);
        } else if(auto f = dyn_cast<FunctionDecl>(Node)){
            Func = const_cast<FunctionDecl*>(f);
        }
        // For each parameter of the function
        for(auto Param : Func->parameters()){
            // Create Match from parameter that bundles it with location&context
            // not clean to create matches like this, but better than extracting
            // matches extra for this from the AST
            unsigned Location = Context->getFullLoc(
                Param->getBeginLoc()).getLineNumber();
            Match<ParmVarDecl> ParmMatch(Location, Param, Context);
            // If parameter pack is used, strip it off.
            QualType qt = Param->getOriginalType();
            if(isParameterPackType(qt))
                // getPattern gives us the the type that is 'being packed'.
                qt = cast<PackExpansionType>(qt.getTypePtr())->getPattern();
            // Get the underlying unqualified type
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
        // Put current function in vector specifying what kind of parameter it has
        addFunction(match, ParmMap);
    }
}

void MoveSemanticsAnalysis::extractFeatures(){
    internal::VariadicDynCastAllOfMatcher<Type, PackExpansionType> packExpansionType;

    auto ftmatcher = functionTemplateDecl(
        isExpansionInMainFile(),
        // Function template should not templatize any of the following:
        unless(has(decl(anyOf(
            cxxConstructorDecl(), // ctors
            functionDecl(hasName("operator="))))))) // assigment operators
            // dtors not necessary, those cannot be templated
    .bind("ft");
    auto ftresult = Extractor.extract2(*Context, ftmatcher);
    auto fts = getASTNodes<FunctionTemplateDecl>(ftresult, "ft");
    associateParameters(fts);

    auto fmatcher = functionDecl(
        isExpansionInMainFile(),
        unless(anyOf(
            cxxConstructorDecl(), // Don't bind to class ctors
            hasName("operator="), // Ignore assignment operators
            cxxDestructorDecl())), // Actually unnecessary, as our analysis
            //  ignores functions (which dtors are) without parameters anyway
        unless(hasParent(functionTemplateDecl()))) // Don't bind to templates
    .bind("f");
    auto fresult = Extractor.extract2(*Context, fmatcher);
    auto fs = getASTNodes<FunctionDecl>(fresult, "f");
    associateParameters(fs);
}
// Issues:
// 1. universal reference cannot be part of function (non-template) decl,
// because the type deduction will not take place at its call site. however,
// a non-template function can be variadic if it is contained e.g. in a class or
// function template
// 2. What about constructors? analyze those too?
// No, separately probably better.

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

void MoveSemanticsAnalysis::analyzeFeatures() {
    extractFeatures();

    gatherData("function decls", "pass by value", FuncsWithValueParm);
    gatherData("function decls", "pass by non-const lvalue ref",
        FuncsWithNonConstLValueRefParm);
    gatherData("function decls", "pass by const lvalue ref",
        FuncsWithConstLValueRefParm);
    gatherData("function decls", "pass by rvalue ref",
        FuncsWithRValueRefParm);

    gatherData("parm decls", "value", ValueParms);
    gatherData("parm decls", "non-const lvalue ref", NonConstLValueRefParms);
    gatherData("parm decls", "const lvalue ref", ConstLValueRefParms);
    gatherData("parm decls", "rvalue ref", RValueRefParms);
    gatherData("parm decls", "universal ref", UniversalRefParms);

    gatherData("function template decls", "pass by value",
        FuncTemplatesWithValueParm);
    gatherData("function template decls", "pass by non-const lvalue ref",
        FuncTemplatesWithNonConstLValueRefParm);
    gatherData("function template decls", "pass by const lvalue ref",
        FuncTemplatesWithConstLValueRefParm);
    gatherData("function template decls", "pass by rvalue ref",
        FuncTemplatesWithRValueRefParm);
    gatherData("function template decls", "pass by universal ref",
        FuncTemplatesWithUniversalRefParm);
}

void MoveSemanticsAnalysis::processJSON(){

}

//-----------------------------------------------------------------------------
