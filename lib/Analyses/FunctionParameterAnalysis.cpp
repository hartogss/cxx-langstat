#include <iostream>

#include "cxx-langstat/Analyses/FunctionParameterAnalysis.h"

#include "cxx-langstat/Utils.h"

using namespace clang;
using namespace clang::ast_matchers;
using json = nlohmann::json;
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

//-----------------------------------------------------------------------------

// For each match, figures out the type of the parameters.
// T should be clang::FunctionDecl or clang::FunctionTemplateDecl
template<typename T>
void FunctionParameterAnalysis::associateParameters(const Matches<T>& Matches){
    // For each function (template)
    for(auto match : Matches){
        auto Node = match.Node;
        clang::FunctionDecl* Func;
        auto WasTemplate = false;
        clang::FunctionTemplateDecl* FTD = nullptr;
        // Object to store relevant info about function (maybe template) that
        // later goes into JSON containing features
        FunctionTemplateInfo Info;
        // If function templates, look at contained function decl.
        if(auto ft = dyn_cast<FunctionTemplateDecl>(Node)){
            Func = ft->getTemplatedDecl();
            WasTemplate = true;
            FTD = const_cast<FunctionTemplateDecl*>(ft);
        } else if(auto f = dyn_cast<FunctionDecl>(Node)){
            Func = const_cast<FunctionDecl*>(f);
        }

        // Printing policy for printing parameter types
        LangOptions LO;
        PrintingPolicy PP(LO);
        // Causes some problems with instantiation-dependent types, namely
        // that no qualifications are printed
        PP.PrintCanonicalTypes = true;
        PP.SuppressTagKeyword = false;
        PP.SuppressScope = false;
        PP.SuppressUnwrittenScope = false;
        PP.FullyQualifiedName = true;

        Info.Location = match.Location;
        Info.Identifier = getMatchDeclName(match);
        Info.Signature = Func->getType().getAsString();

        // For each parameter of the function
        for(auto Param : Func->parameters()){

            auto ParmType = Param->getType();
            ParmInfo PInfo(Context->getFullLoc(Param->getBeginLoc())
                .getLineNumber(),
                cast<NamedDecl>(Param)->getQualifiedNameAsString(),
                ParmType.getAsString(PP));
            PInfo.isInstantiationDependent = ParmType
                .getTypePtr()->isInstantiationDependentType();
            // If parameter pack is used, strip it off.
            // For pack expansion T..., below function gives use the pattern T.
            ParmType = ParmType.getNonPackExpansionType();
            // Get the underlying unqualified type
            const Type* type = ParmType.getTypePtr();
            // Split into value, lvalue ref and rvalue ref groups.
            if(type->isLValueReferenceType()){
                if(type->getPointeeType().isConstQualified()){
                    Info.ByConstLvalueRef++;
                    PInfo.Kind = ParmKind::ConstLValueRef;
                }else{
                    Info.ByNonConstLvalueRef++;
                    PInfo.Kind = ParmKind::NonConstLValueRef;
                }
            }else if(type->isRValueReferenceType()){
                if(isUniversalReference(FTD, ParmType) && WasTemplate){
                    Info.ByUniversalRef++;
                    PInfo.Kind = ParmKind::UniversalRef;
                }else{
                    Info.ByRvalueRef++;
                    PInfo.Kind = ParmKind::RValueRef;
                }
            }else{
                Info.ByValue++;
                PInfo.Kind = ParmKind::Value;
            }
            Parms.emplace_back(PInfo);
        }
        if(WasTemplate)
            FunctionTemplates.emplace_back(Info);
        else
            Functions.emplace_back(Info); // info specific to templates will be cast away automatically
    }
}

void FunctionParameterAnalysis::extractFeatures(){
    internal::VariadicDynCastAllOfMatcher<Type, PackExpansionType> packExpansionType;

    auto ftmatcher = functionTemplateDecl(
        isExpansionInMainFile(),
        unless(has(decl(anyOf(
            cxxConstructorDecl(), // Ignore ctors
            functionDecl(hasName("operator="))))))) // Ignore assigment operators
            // Don't have to ignore dtors, those cannot be templated
    .bind("ft");
    auto ftresult = Extractor.extract2(*Context, ftmatcher);
    auto fts = getASTNodes<FunctionTemplateDecl>(ftresult, "ft");
    associateParameters(fts);

    auto fmatcher = functionDecl(
        isExpansionInMainFile(),
        unless(anyOf(
            cxxConstructorDecl(), // Ignore ctors
            hasName("operator="), // Ignore assignment operators
            cxxDestructorDecl())), // Ignore dtors
        // Ignore functions from function templates, but don't ignore function
        // template instantiations
        anyOf(unless(hasParent(functionTemplateDecl())),
            isTemplateInstantiation()))
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

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(FunctionInfo, Location, Identifier,
    ByValue, ByNonConstLvalueRef, ByConstLvalueRef, ByRvalueRef, Signature);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(FunctionTemplateInfo, Location, Identifier,
    ByValue, ByNonConstLvalueRef, ByConstLvalueRef, ByRvalueRef, ByUniversalRef,
    Signature);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ParmInfo, Location, Identifier, Kind, Type,
    isInstantiationDependent);

template<typename T>
void FunctionParameterAnalysis::gatherData(std::string DeclKind, const std::vector<T>& fs){
        for(auto f : fs){
            json f_j = f;
            Features[DeclKind].emplace_back(f_j);
        }
}

void FunctionParameterAnalysis::analyzeFeatures() {
    extractFeatures();
    // Fill JSON with data about function (templates)
    gatherData("functions", Functions);
    gatherData("function templates", FunctionTemplates);
    gatherData("parameters", Parms);
}

//
void FunctionsCount(ordered_json& Stats, ordered_json j, bool templated){
    unsigned Value=0, NCLValueRef=0, CLValueRef=0, RValueRef = 0, UniversalRef=0;
    std::string addendum = "";
    for(const auto& f_j : j){
        // std::cout << f_j.dump(4) << std::endl;
        if(templated) {
            addendum = " template";
            FunctionTemplateInfo ft;
            from_json(f_j, ft);
            if(ft.ByValue)
                Value++;
            if(ft.ByNonConstLvalueRef)
                NCLValueRef++;
            if(ft.ByConstLvalueRef)
                CLValueRef++;
            if(ft.ByRvalueRef)
                RValueRef++;
            if(ft.ByUniversalRef)
                UniversalRef++;
        } else {
            FunctionInfo f;
            from_json(f_j, f);
            if(f.ByValue)
                Value++;
            if(f.ByNonConstLvalueRef)
                NCLValueRef++;
            if(f.ByConstLvalueRef)
                CLValueRef++;
            if(f.ByRvalueRef)
                RValueRef++;
        }
    }
    auto desc = "count for each kind of parameter how many function" + addendum + " use them";
    Stats[desc]["value"] = Value;
    Stats[desc]["non-const lvalue ref"] = NCLValueRef;
    Stats[desc]["const lvalue ref"] = CLValueRef;
    Stats[desc]["rvalue ref"] = RValueRef;
    if(templated)
        Stats[desc]["universal ref"] = UniversalRef;
}
void ParamsCount(ordered_json& Stats, ordered_json j){
    unsigned Value=0, NCLValueRef=0, CLValueRef=0, RValueRef = 0, UniversalRef=0;
    for(const auto& p_j : j){
        // std::cout << p_j.dump(4) << std::endl;
        ParmInfo p;
        from_json(p_j, p);
        switch (p.Kind) {
            case ParmKind::Value:
                Value++;
                break;
            case ParmKind::NonConstLValueRef:
                NCLValueRef++;
                break;
            case ParmKind::ConstLValueRef:
                CLValueRef++;
                break;
            case ParmKind::RValueRef:
                RValueRef++;
                break;
            case ParmKind::UniversalRef:
                UniversalRef++;
                break;
        }
    }
    auto desc = "parameter kind prevalence";
    Stats[desc]["value"] = Value;
    Stats[desc]["non-const lvalue ref"] = NCLValueRef;
    Stats[desc]["const lvalue ref"] = CLValueRef;
    Stats[desc]["rvalue ref"] = RValueRef;
    Stats[desc]["universal ref"] = UniversalRef;
}

void FunctionParameterAnalysis::processFeatures(ordered_json j){
    if(j.contains("functions"))
        FunctionsCount(Statistics, j.at("functions"), false);
    if(j.contains("function templates"))
        FunctionsCount(Statistics, j.at("function templates"), true);
    if(j.contains("parameters"))
        ParamsCount(Statistics, j.at("parameters"));
}

//-----------------------------------------------------------------------------
