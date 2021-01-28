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

void TemplateParameterAnalysis::gatherStatistics(const Matches<Decl>& Matches,
    std::string TemplateKind){
    ordered_json Templates;
    for(auto match : Matches){
        ordered_json Template;
        Template["location"] = match.Location;
        auto TParms = cast<TemplateDecl>(match.Node)->getTemplateParameters();
        Template["uses param pack"] = TParms->hasParameterPack();
        std::map<std::string, unsigned> ParmKindCounts = {
            {"non-type",0},{"type",0},{"template",0},};
        for(unsigned idx=0; idx<TParms->size(); idx++)
            ParmKindCounts[getTemplateParmKind(TParms->getParam(idx))]++;
        Template["parameters"]["non-type"] = ParmKindCounts["non-type"];
        Template["parameters"]["type"] = ParmKindCounts["type"];
        Template["parameters"]["template"] = ParmKindCounts["template"];
        Templates[getMatchDeclName(match)].emplace_back(Template);
    }
    Features[TemplateKind] = Templates;
}

void TemplateParameterAnalysis::analyzeFeatures(){
    extractFeatures();
    gatherStatistics(ClassTemplates, "class templates");
    gatherStatistics(FunctionTemplates, "function templates");
    gatherStatistics(VariableTemplates, "variable templates");
    gatherStatistics(AliasTemplates, "alias templates");
}


void TemplateParameterAnalysis::processFeatures(nlohmann::ordered_json j){

}

//-----------------------------------------------------------------------------
