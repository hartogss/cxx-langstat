#include <iostream>
#include <vector>

#include <nlohmann/json.hpp>

#include "cxx-langstat/Analyses/TemplateParameterAnalysis.h"
#include "cxx-langstat/Utils.h"

using namespace clang;
using namespace clang::ast_matchers;
using ordered_json = nlohmann::ordered_json;

//-----------------------------------------------------------------------------
// For each kind of template (class, function, variable, alias) individually:
// √ How many templates use parameter packs/are variadic?
// √ Give distribution of the parameters used: nontype, type, template.

// TODO: Other possible statistics:
// - How many templates use template template parameters?
// - Statistics on parameter pack 'contents'.
// - Apply TPA to template template parameters.


void TemplateParameterAnalysis::extractFeatures(){

    // Should be defined already according to matcher reference, but doesn't
    // compile nor work in clang-query. Should file bug report soon.
    internal::VariadicDynCastAllOfMatcher<Decl, TemplateTemplateParmDecl>
    templateTemplateParmDecl;

    internal::VariadicDynCastAllOfMatcher<Decl, VarTemplateDecl> varTemplateDecl;

    auto CTResults = Extractor.extract2(*Context,
        classTemplateDecl(isExpansionInMainFile())
    .bind("ct"));
    ClassTemplates = getASTNodes<Decl>(CTResults, "ct");

    auto FTResults = Extractor.extract2(*Context,
        functionTemplateDecl(isExpansionInMainFile())
    .bind("ft"));
    FunctionTemplates = getASTNodes<Decl>(FTResults, "ft");

    auto VTResults = Extractor.extract2(*Context,
        varTemplateDecl(isExpansionInMainFile())
    .bind("vt"));
    VariableTemplates = getASTNodes<Decl>(VTResults, "vt");

    auto ATResults = Extractor.extract2(*Context,
        typeAliasTemplateDecl(isExpansionInMainFile())
    .bind("at"));
    AliasTemplates = getASTNodes<Decl>(ATResults, "at");
}

std::string getTemplateParmKind(NamedDecl* D){
    auto kind = cast<Decl>(D)->getDeclKindName();
    if(strcmp(kind, "NonTypeTemplateParm") == 0)
        return "non-type";
    else if(strcmp(kind, "TemplateTypeParm") == 0)
        return "type";
    else if(strcmp(kind, "TemplateTemplateParm") == 0)
        return "template";
    else {
        std::cout << "invalid template parameter kind!\n";
        return "?";
    }
}

void TemplateParameterAnalysis::gatherStatistics(){
    ordered_json CTs;
    for(auto match : ClassTemplates){
        ordered_json CT;
        CT["location"] = match.location;
        auto TParms = cast<TemplateDecl>(match.node)->getTemplateParameters();
        CT["uses param pack"] = TParms->hasParameterPack();
        std::map<std::string, unsigned> ParmKindCounts = {
            {"non-type",0},{"type",0},{"template",0},};
        for(unsigned idx=0; idx<TParms->size(); idx++)
            ParmKindCounts[getTemplateParmKind(TParms->getParam(idx))]++;
        CT["parameters"]["non-type"] = ParmKindCounts["non-type"];
        CT["parameters"]["type"] = ParmKindCounts["type"];
        CT["parameters"]["template"] = ParmKindCounts["template"];
        CTs[getMatchDeclName(match)] = CT;
    }
    ordered_json FTs;
    for(auto match : FunctionTemplates){
        ordered_json FT;
        FT["location"] = match.location;
        auto TParms = cast<TemplateDecl>(match.node)->getTemplateParameters();
        FT["uses param pack"] = TParms->hasParameterPack();
        std::map<std::string, unsigned> ParmKindCounts = {
            {"non-type",0},{"type",0},{"template",0},};
        for(unsigned idx=0; idx<TParms->size(); idx++)
            ParmKindCounts[getTemplateParmKind(TParms->getParam(idx))]++;
        FT["parameters"]["non-type"] = ParmKindCounts["non-type"];
        FT["parameters"]["type"] = ParmKindCounts["type"];
        FT["parameters"]["template"] = ParmKindCounts["template"];
        FTs[getMatchDeclName(match)] = FT;
    }
    ordered_json VTs;
    for(auto match : VariableTemplates){
        ordered_json VT;
        VT["location"] = match.location;
        auto TParms = cast<TemplateDecl>(match.node)->getTemplateParameters();
        VT["uses param pack"] = TParms->hasParameterPack();
        std::map<std::string, unsigned> ParmKindCounts = {
            {"non-type",0},{"type",0},{"template",0},};
        for(unsigned idx=0; idx<TParms->size(); idx++)
            ParmKindCounts[getTemplateParmKind(TParms->getParam(idx))]++;
        VT["parameters"]["non-type"] = ParmKindCounts["non-type"];
        VT["parameters"]["type"] = ParmKindCounts["type"];
        VT["parameters"]["template"] = ParmKindCounts["template"];
        VTs[getMatchDeclName(match)] = VT;
    }
    ordered_json ATs;
    for(auto match : AliasTemplates){
        ordered_json AT;
        AT["location"] = match.location;
        auto TParms = cast<TemplateDecl>(match.node)->getTemplateParameters();
        AT["uses param pack"] = TParms->hasParameterPack();
        std::map<std::string, unsigned> ParmKindCounts = {
            {"non-type",0},{"type",0},{"template",0},};
        for(unsigned idx=0; idx<TParms->size(); idx++)
            ParmKindCounts[getTemplateParmKind(TParms->getParam(idx))]++;
        AT["parameters"]["non-type"] = ParmKindCounts["non-type"];
        AT["parameters"]["type"] = ParmKindCounts["type"];
        AT["parameters"]["template"] = ParmKindCounts["template"];
        ATs[getMatchDeclName(match)] = AT;
    }
    Result["class templates"] = CTs;
    Result["function templates"] = FTs;
    Result["variable templates"] = VTs;
    Result["alias templates"] = ATs;
}

void TemplateParameterAnalysis::analyzeFeatures(){
        extractFeatures();
        gatherStatistics();
}
void TemplateParameterAnalysis::processJSON(){

}

//-----------------------------------------------------------------------------
