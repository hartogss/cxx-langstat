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
// - Statistics on parameter pack "contents".
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

//-----------------------------------------------------------------------------
// Functions to convert structs to/from JSON.
void to_json(nlohmann::json& j, const TemplateParms& p){
    j = nlohmann::json{
        {"non-type", p.Nontype},
        {"type", p.Type},
        {"template", p.Template}
    };
}
void from_json(const nlohmann::json& j, TemplateParms& p){
    j.at("non-type").get_to(p.Nontype);
    j.at("type").get_to(p.Type);
    j.at("template").get_to(p.Template);
}
void to_json(nlohmann::json& j, const Template& t){
    j = nlohmann::json{
        {"location", t.Location},
        {"uses param pack", t.UsesParamPack},
        {"parameters", t.Parms}
    };
}
void from_json(const nlohmann::json& j, Template& t){
    j.at("location").get_to(t.Location);
    j.at("uses param pack").get_to(t.UsesParamPack);
    j.at("parameters").get_to(t.Parms);
}
//-----------------------------------------------------------------------------

void TemplateParameterAnalysis::gatherData(const Matches<Decl>& Matches,
    std::string TemplateKind){
    LangOptions LO;
    PrintingPolicy PP(LO);
    PP.PrintCanonicalTypes = true;
    PP.SuppressTagKeyword = false;
    PP.SuppressScope = false;
    PP.SuppressUnwrittenScope = true;
    PP.FullyQualifiedName = true;
    PP.Bool = true;
    ordered_json Templates;
    for(auto match : Matches){
        Template Template;
        TemplateParms Parms;
        Template.Location = match.Location;
        auto TParms = cast<TemplateDecl>(match.Node)->getTemplateParameters();
        // std::cout << TParms->size() << std::endl;
        Template.UsesParamPack = TParms->hasParameterPack();
        for(unsigned idx=0; idx<TParms->size(); idx++){
            auto kind = cast<Decl>(TParms->getParam(idx))->getDeclKindName();
            // std::cout << kind << std::endl;
            if(strcmp(kind, "NonTypeTemplateParm") == 0)
                Parms.Nontype++;
            else if(strcmp(kind, "TemplateTypeParm") == 0)
                Parms.Type++;
            else if(strcmp(kind, "TemplateTemplateParm") == 0)
                Parms.Template++;
            else {
                std::cout << "invalid template parameter kind!\n";
                exit(1);
            }
        }
        assert(Parms.NumParms() != 0);
        Template.Parms = Parms;
        nlohmann::json JSONTemplate = Template;
        Templates[match.getDeclName(PP)].emplace_back(JSONTemplate);
    }
    Features[TemplateKind] = Templates;
}

void TemplateParameterAnalysis::analyzeFeatures(){
    extractFeatures();
    gatherData(ClassTemplates, "class templates");
    gatherData(FunctionTemplates, "function templates");
    gatherData(VariableTemplates, "variable templates");
    gatherData(AliasTemplates, "alias templates");
}

void useParamPacks(ordered_json& Stats, ordered_json j){
    for(const auto& [TemplateKind, TemplateNames] : j.items()){
        unsigned usePP = 0;
        unsigned dontUsePP = 0;
        for(const auto& TemplateName : TemplateNames){
            for(const auto& JSONTemplate : TemplateName){
                // std::cout << JSONTemplate.dump(4) << std::endl;
                // Template Template = JSONTemplate.get<Template>(); doesn't work?
                Template Template;
                from_json(JSONTemplate, Template);
                if(Template.UsesParamPack)
                    usePP++;
                else
                    dontUsePP++;
            }
        }
        auto desc = "parameter pack usage";
        // if(usePP + dontUsePP){
            Stats[desc][TemplateKind]["yes"] = usePP;
            Stats[desc][TemplateKind]["no"] = dontUsePP;
        // }
    }
}

void TemplateParameterAnalysis::processFeatures(nlohmann::ordered_json j){
    useParamPacks(Statistics, j);

}

//-----------------------------------------------------------------------------
