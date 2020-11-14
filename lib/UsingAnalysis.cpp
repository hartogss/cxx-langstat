#include <iostream>
#include <vector>

#include "cxx-langstat/UsingAnalysis.h"

using namespace clang::ast_matchers;

//-----------------------------------------------------------------------------
// Question: How many typedef, how many aliases?
// How many typedefs have been abandoned because of the ease of alias templates?


UsingAnalysis::UsingAnalysis(clang::ASTContext& Context) : Analysis(Context){

}
void UsingAnalysis::extract() {
    auto m1 = typedefDecl(isExpansionInMainFile()).bind("typedef");
    auto m2 = typeAliasTemplateDecl().bind("using");
    TypeDefDecls = Extr.extract("typedef", m1);
    TypeAliasDecls = Extr.extract("using", m2);
    analyze();
}
void UsingAnalysis::analyze(){
    std::cout << "Typedecls found:\n";
    for(auto m : TypeDefDecls){
        std::cout
        << dyn_cast<clang::NamedDecl>(m.node)->getNameAsString()
        << " @ " << m.location << std::endl;
    }
    std::cout << "Type aliases (\"using\") found:\n";
    for(auto m : TypeAliasDecls){
        std::cout << dyn_cast<clang::NamedDecl>(m.node)->getNameAsString()
        << " @ " << m.location << std::endl;
    }

}
void UsingAnalysis::run(){
    std::cout << "\033[32mRunning UsingAnalysis:\033[0m" << std::endl;
    extract();
}

//-----------------------------------------------------------------------------
