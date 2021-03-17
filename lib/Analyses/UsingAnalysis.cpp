#include <iostream>
#include <vector>

#include "cxx-langstat/Analyses/UsingAnalysis.h"
#include "cxx-langstat/Utils.h"

using namespace clang;
using namespace clang::ast_matchers;
using ordered_json = nlohmann::ordered_json;

//-----------------------------------------------------------------------------
// Question: Did programmers abandon typedef in favor of aliases (e.g.
// using newType = oldType<int> )?
// What's the reason for that development?
// Is it because alias templates are easier to use than "typedef templates?"?
// Could also be because "typedef templates" require typename or ::type to be
// used.

void UsingAnalysis::extractFeatures() {
    // Synonyms.clear();

    // Type aliases
    // however, only those that are not part of type alias
    // templates. Type alias template contains type alias node in clang AST.
    auto typeAlias = typeAliasDecl(
        isExpansionInMainFile(),
        unless(hasParent(typeAliasTemplateDecl())))
    .bind("alias");

    // Alias template
    auto typeAliasTemplate = typeAliasTemplateDecl(
        isExpansionInMainFile())
    .bind("aliastemplate");

    // "Typedef template"
    // Of course there are no typedef templates in C++, but we consider the
    // following idiom mostly used prior to C++11 to be a "typedef template":
    // template<typename T>
    // struct Pair {
    //     typedef Tuple<T> type; // don't have to name this 'type'
    // };
    // We thus say that a class template is a "typedef decl" if it contains only
    // typedefs, nothing else.
    // No requirement about the access specifier of the typedef.
    auto typedefTemplate = classTemplateDecl(has(cxxRecordDecl(
        isExpansionInMainFile(),
        // Must have typedefDecl
        // has() will ensure only first typedef is matched. if want to allow
        // for multiple typedefs in a template, we have to use forEach()
        forEach(typedefDecl().bind("td")),
        // Must not have decl that is not typedef, access specifier or cxxrecord
        unless(has(decl(
            unless(anyOf(
                typedefDecl(),
                accessSpecDecl(),
                // Must be allowed to contain implicit cxxrecord,
                // instrinsic to clang AST. Same as with VTA.
                cxxRecordDecl(isImplicit())))))))))
    .bind("typedeftemplate");

    // Typedef
    // Count all typedefs that are explicitly written by programmer (high level)
    // Concretely, that means:
    // We want typedefs matched here not to be part of a "typedef template",
    // because we want to study those separately. To do that, have 2 requirements:
    // 1) a typedef matched here may not be part of a class template that is
    // used to create a "typedef template"
    // 2) a typedef matched here may not be part of a class template specialization
    // that occurs when the programmer wants to make use of the "typedef template"
    auto typedef_ = typedefDecl(
        isExpansionInMainFile(),
        // 2) Don't match typedef in class specializations coming from "typedef templates"
        // 1) will filtered out later
        unless(hasParent(classTemplateSpecializationDecl(
            hasSpecializedTemplate(typedefTemplate)))))
    .bind("typedef");

    // Note: Left works only in clang-query, right works in both CQ and LibTooling
    // has(anyOf(...)) -> has(decl(anyOf(...)))
    // has(unless(...)) -> has(decl(unless(...)))

    Matches<clang::Decl> TypedefDecls = Extractor.extract(*Context, "typedef", typedef_);
    Matches<clang::Decl> TypeAliasDecls = Extractor.extract(*Context, "alias", typeAlias);
    auto typedeftemplateresults = Extractor.extract2(*Context, typedefTemplate);
    Matches<clang::Decl> TypedefTemplateDecls = getASTNodes<Decl>(typedeftemplateresults, "typedeftemplate");
    Matches<clang::Decl> td = getASTNodes<Decl>(typedeftemplateresults, "td");
    Matches<clang::Decl> TypeAliasTemplateDecls = Extractor.extract(*Context, "aliastemplate",
        typeAliasTemplate);

    // Filters as described in 1) in typedef
    // need to do extra work to remove from typedefdecls those decls that occur
    // in typedeftemplatedecls (to get distinction between typedef and typedef
    // templates)
    for(auto decl : td){
        for(unsigned i=0; i<TypedefDecls.size(); i++){
            auto d = TypedefDecls[i];
            if (decl == d)
                TypedefDecls.erase(TypedefDecls.begin()+i);
        }
    }
    // Possible improvement: for each typedef/alias, state what type was aliased
    for(auto m : TypedefDecls)
        Synonyms.emplace_back(Synonym(m.Location, Typedef, false));
    for(auto m : TypeAliasDecls)
        Synonyms.emplace_back(Synonym(m.Location, Alias, false));
    // possible improvement: instead of stating a "typedef template" multiple
    // times when it contains multiple typdefs, state it a single time, but
    // have it have a number showing #typedefs it contains
    for(auto m : TypedefTemplateDecls)
        Synonyms.emplace_back(Synonym(m.Location, Typedef, true));
    for(auto m : TypeAliasTemplateDecls)
        Synonyms.emplace_back(Synonym(m.Location, Alias, true));
}

//-----------------------------------------------------------------------------
// Helper functions to convert SynonymKind to string and back.
std::string SKToString(SynonymKind Kind){
    switch (Kind) {
        case Typedef:
            return "Typedef";
        case Alias:
            return "Alias";
        default:
            return "invalid";
    }
}
SynonymKind getSKFromString(llvm::StringRef s){
    if(s.equals("Typedef"))
        return Typedef;
    else
        return Alias;
}
// Functions to convert structs to/from JSON.
void to_json(nlohmann::json& j, const Synonym& s){
    j = nlohmann::json{
        {"location", s.Location},
        {"kind", SKToString(s.Kind)},
        {"templated", s.Templated}
    };
}
void from_json(const nlohmann::json& j, Synonym& s){
    j.at("location").get_to(s.Location);
    s.Kind = getSKFromString(j.at("kind").get<std::string>());
    j.at("templated").get_to(s.Templated);
}

//-----------------------------------------------------------------------------
//
void UsingAnalysis::analyzeFeatures(){
    extractFeatures();
    for(auto s : Synonyms){
        nlohmann::json s_j = s;
        Features.emplace_back(s_j);
    }
}

// Computes prevalences of typedefs and aliases (and templates thereof).
void SynonymPrevalence(ordered_json& Stats, ordered_json j){
    unsigned Typedefs=0, Aliases=0, TypedefTemplates=0, AliasTemplates = 0;
    for(const auto& s_j : j){
        // std::cout << s_j.dump(4) << std::endl;
        Synonym s;
        from_json(s_j, s);
        if(s.Kind == Typedef){
            if(s.Templated)
                TypedefTemplates++;
            else
                Typedefs++;
        } else if(s.Kind == Alias){
            if(s.Templated)
                AliasTemplates++;
            else
                Aliases++;
        }
    }
    auto desc = "prevalence of typedef/using";
    Stats[desc]["typedef"] = Typedefs;
    Stats[desc]["alias"] = Aliases;
    Stats[desc]["typedef template"] = TypedefTemplates;
    Stats[desc]["alias templates"] = AliasTemplates;
}

void UsingAnalysis::processFeatures(nlohmann::ordered_json j){
    SynonymPrevalence(Statistics, j);
}

//-----------------------------------------------------------------------------
