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
    // Count all typedefs that are explicitly written by programmer (high level)
    // Concretely, that means:
    // - in main file
    // - not generated automatically in class template specializations
    // - not part of instantiations
    // Additionally: don't want them to be part of "template typedefs", in
    // our statistics want to know how many were templatized & how many were not
    auto typedef_ = typedefDecl(
        isExpansionInMainFile(),
        unless(isInstantiated())) // I think fixes s.t. specializations are
                                  // ignored, got idea from UseUsingCheck.cpp,
                                  // which is from clang-tidy
    .bind("typedef");

    // Type aliases, however, only those that are not part of type alias
    // templates. Type alias template contains type alias node in clang AST.
    auto typeAlias = typeAliasDecl(
        isExpansionInMainFile(),
        unless(hasParent(typeAliasTemplateDecl())))
    .bind("alias");

    // Of course there are no typedef templates in C++, but we consider the
    // following idiom mostly used prior to C++11 to be a "typedef template":
    // template<typename T>
    //     struct Pair {
    // typedef Tuple<T> type; // don't have to name this 'type'
    // };
    // We thus say that a class template is a "typedef decl" if it contains only
    // typedefs, nothing else.
    // Requires the typedef in the template to be public.
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
                cxxRecordDecl())))))))) // Must be allowed to contain cxxrecord,
                                        // instrinsic to clang AST
    .bind("typedeftemplate");

    // Alias template
    auto typeAliasTemplate = typeAliasTemplateDecl(
        isExpansionInMainFile())
    .bind("aliastemplate");

    // Note: Left works only in clang-query, right works in both CQ and LibTooling
    // has(anyOf(...)) -> has(decl(anyOf(...)))
    // has(unless(...)) -> has(decl(unless(...)))

    TypedefDecls = Extractor.extract(*Context, "typedef", typedef_);
    TypeAliasDecls = Extractor.extract(*Context, "alias", typeAlias);
    auto typedeftemplateresults = Extractor.extract2(*Context, typedefTemplate);
    TypedefTemplateDecls = getASTNodes<Decl>(typedeftemplateresults, "typedeftemplate");
    td = getASTNodes<Decl>(typedeftemplateresults, "td");
    TypeAliasTemplateDecls = Extractor.extract(*Context, "aliastemplate",
        typeAliasTemplate);

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
}
template<typename T>
void UsingAnalysis::gatherData(std::string RaccourciKind, const Matches<T>& Matches){
    // Raccourci: either typedef or alias, possibly "templated"
    // Possible improvement: for each typedef/alias, state what type was aliased
    // possible improvement: state all typedefs of a typedef template, since
    // it can contain multiple
    ordered_json Raccourcis;
    for(auto match : Matches){
        ordered_json Raccourci;
        Raccourci["location"] = match.Location;
        Raccourcis[getMatchDeclName(match)] = Raccourci;
    }
    Features[RaccourciKind] = Raccourcis;
}
void UsingAnalysis::analyzeFeatures(){
    extractFeatures();
    gatherData("typedefs", TypedefDecls);
    gatherData("aliases", TypeAliasDecls);
    gatherData("typedef templates", TypedefTemplateDecls);
    gatherData("alias templates", TypeAliasTemplateDecls);
}
void UsingAnalysis::processFeatures(nlohmann::ordered_json j){

}

//-----------------------------------------------------------------------------
