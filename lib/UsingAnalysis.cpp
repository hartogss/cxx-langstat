#include <iostream>
#include <vector>

#include "cxx-langstat/UsingAnalysis.h"
#include "cxx-langstat/Utils.h"

using namespace clang::ast_matchers;

//-----------------------------------------------------------------------------
// Question: Did programmers abandon typedef in favor of aliases (e.g.
// using newType = oldType<int> )?
// What's the reason for that development?
// Is it because alias templates are easier to use than "typedef templates?"?
// Could also be because "typedef templates" require typename or ::type to be
// used.

UsingAnalysis::UsingAnalysis(clang::ASTContext& Context) : Analysis(Context){

}
void UsingAnalysis::extract() {
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
    auto typeAlias = typeAliasDecl(unless(hasParent(typeAliasTemplateDecl())))
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

    //
    auto typeAliasTemplate = typeAliasTemplateDecl().bind("aliastemplate");

    // Note: Left works only in clang-query, right works in both CQ and LibTooling
    // has(anyOf(...)) -> has(decl(anyOf(...)))
    // has(unless(...)) -> has(decl(unless(...)))

    // auto m2 = decl(unless(typeAliasTemplateDecl())).bind("using"); //causes matches.size() to be 1, but for (auto) over that lists prints a lot more

    TypedefDecls = Extr.extract("typedef", typedef_);
    TypeAliasDecls = Extr.extract("alias", typeAlias);
    TypedefTemplateDecls = Extr.extract("typedeftemplate", typedefTemplate);
    TypeAliasTemplateDecls = Extr.extract("aliastemplate", typeAliasTemplate);

    // need to do extra work to remove from typedefdecls those decls that occur
    // in typedeftemplatedecls (to get distinciton between typedef and typedef templates)
    td = Extr.extract("td", typedefTemplate);
    for(auto decl : td){
        for(unsigned i=0; i<TypedefDecls.size(); i++){
            auto d = TypedefDecls[i];
            if (decl == d)
                TypedefDecls.erase(TypedefDecls.begin()+i);
        }
    }

    analyze();
}
void UsingAnalysis::analyze(){
    printStatistics("Typedef found", TypedefDecls);
    printStatistics("Type aliases found", TypeAliasDecls);
    printStatistics("\"Typedef templates\" found", TypedefTemplateDecls);
    printStatistics("Typedefs from \"Typedef templates\"", td);
    printStatistics("Type alias templates found", TypeAliasTemplateDecls);
}
void UsingAnalysis::run(){
    std::cout << "\033[32mRunning UsingAnalysis:\033[0m" << std::endl;
    extract();
}

//-----------------------------------------------------------------------------
