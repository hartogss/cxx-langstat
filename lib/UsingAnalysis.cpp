#include <iostream>
#include <vector>

#include "cxx-langstat/UsingAnalysis.h"

using namespace clang::ast_matchers;

//-----------------------------------------------------------------------------
// Question: Did programmers abandon typedef in favor of aliases?
// What's the reason for that development?
// Is it because templatized aliases are easier to use than templatized typedefs?
// Could also be because templatized typedefs require ::type, but that usage
// we cannot analyze.


UsingAnalysis::UsingAnalysis(clang::ASTContext& Context) : Analysis(Context){

}
void UsingAnalysis::extract() {
    // Want typedef to be in main file & not to be in a struct or class
    // declaration because in that case could be that is a templatized typedef
    // is this senseful to do? it is possible to just use a typedef inside of
    // a class for other reasons
    auto m1 = typedefDecl(
        isExpansionInMainFile(),
        unless(hasAncestor(classTemplateDecl())))
        .bind("typedef");
    // auto m2 = decl(unless(typeAliasTemplateDecl())).bind("using"); //causes matches result size to be 1, but for (auto) over that lists prints a lot more
    auto m2 = typeAliasDecl().bind("using");
    auto m3 = typeAliasTemplateDecl().bind("usingtemplate");


    // auto only =
    // auto m = forStmt(has(forStmt()));
    // DeclarationMatcher m4 = classTemplateDecl(has(
    //     cxxRecordDecl(
    //         has(typedefDecl()), // must have typedefDecl
    //         // unless(has(unless(anyOf(typedefDecl(), cxxRecordDecl()))))
    //     ) // but mustn't have something that is neither typedef nor cxxrecord
    // )).bind("typedeftemplatized");

    // classTemplateDecl(has(cxxRecordDecl(unless(has(unless(anyOf(typedefDecl(), cxxRecordDecl())))), has(typedefDecl()), has(cxxRecordDecl()))))

    auto m7 = cxxRecordDecl(has(unless(cxxRecordDecl())));


    TypeDefDecls = Extr.extract("typedef", m1);
    TypeAliasDecls = Extr.extract("using", m2);
    TypeAliasTemplateDecls = Extr.extract("usingtemplate", m3);
    analyze();
}
void UsingAnalysis::analyze(){
    std::cout << "\033[33mTypedecls found:\033[0m\n";
    for(auto m : TypeDefDecls){
        std::cout
        << dyn_cast<clang::NamedDecl>(m.node)->getNameAsString()
        << " @ " << m.location << std::endl;
    }
    std::cout << "\033[33mType aliases (\"using\") found:\033[0m\n";
    // std::cout << TypeAliasTemplateDecls.size() << std::endl;
    for(Match<clang::Decl> m : TypeAliasDecls){
        if (auto n = dyn_cast<clang::NamedDecl>(m.node)){
            std::cout
            << n->getNameAsString()
            << " @ " << m.location << std::endl;
        }
    }
    std::cout << "\033[33mType alias templates (\"using\") found:\033[0m\n";
    for(auto m : TypeAliasTemplateDecls){
        std::cout << dyn_cast<clang::NamedDecl>(m.node)->getNameAsString()
        << " @ " << m.location << std::endl;
    }

}
void UsingAnalysis::run(){
    std::cout << "\033[32mRunning UsingAnalysis:\033[0m" << std::endl;
    extract();
}

//-----------------------------------------------------------------------------
