#include <iostream>
#include <vector>

#include "cxx-langstat/UsingAnalysis.h"

using namespace clang::ast_matchers;

//-----------------------------------------------------------------------------

std::string getDeclName(Match<clang::Decl> node){
    if(auto n = dyn_cast<clang::NamedDecl>(node.node)){
        return n->getNameAsString();
    } else {
        std::cout << "Decl @ " << node.location << "cannot be resolved" << std::endl;
        return "INVALID";
    }
}

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
    // Want typedef to be in main file
    auto typedef_ = typedefDecl(isExpansionInMainFile()).bind("typedef");
    // Type aliases, however, only those that are not part of type alias
    // templates. Type alias template contains type alias node in clang AST.
    auto typeAlias = typeAliasDecl(unless(hasParent(typeAliasTemplateDecl())))
    .bind("alias");
    // Of course there are no typedef templates in C++, but we consider the
    // following idiom mostly used prior to C++11 to be a "typedef template":
    // template<typename T>
    // struct Pair {
    // typedef Tuple<T> type; // don't have to name this 'type'
    // };
    // We thus say that a class template is a "typedef decl" if it contains only
    // typedefs, nothing else.
    // Requires the typedef in the template to be public.
    auto typedefTemplate = classTemplateDecl(has(cxxRecordDecl(
        // Must have typedefDecl
        // has() will ensure only first typedef is matched. if want to allow
        // for multiple typedefs in a template, we have to use forEach()
        has(typedefDecl(isPublic()).bind("td")),
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
    td = Extr.extract("td", typedefTemplate);
    TypeAliasTemplateDecls = Extr.extract("aliastemplate", typeAliasTemplate);

    analyze();
}
void UsingAnalysis::analyze(){
    std::cout << "\033[33mTypedef found:\033[0m\n";
    for(auto m : TypedefDecls){
        std::cout << getDeclName(m)
        << " @ " << m.location << std::endl;
    }
    std::cout << "\033[33mType aliases found:\033[0m\n";
    for(Match<clang::Decl> m : TypeAliasDecls){
        std::cout << getDeclName(m)
        << " @ " << m.location << std::endl;
    }
    std::cout << "\033[33m\"Typedef templates\" found:\033[0m\n";
    for(auto m : TypedefTemplateDecls){
        std::cout << getDeclName(m)
        << " @ " << m.location << std::endl;
    }
    std::cout << "\033[33m\"Typedef templates\" found:\033[0m\n";
    for(auto m : td){
        std::cout << getDeclName(m)
        << " @ " << m.location << std::endl;
    }
    std::cout << "\033[33mType alias templates found:\033[0m\n";
    for(auto m : TypeAliasTemplateDecls){
        std::cout << getDeclName(m)
        << " @ " << m.location << std::endl;
    }
}
void UsingAnalysis::run(){
    std::cout << "\033[32mRunning UsingAnalysis:\033[0m" << std::endl;
    extract();
}

//-----------------------------------------------------------------------------
