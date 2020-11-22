#include <iostream>
#include <vector>

#include "cxx-langstat/StdlibAnalysis.h"

using namespace clang;
using namespace clang::ast_matchers;

//-----------------------------------------------------------------------------

std::string getDeclName(Match<clang::Decl> m){
    if(auto n = dyn_cast<clang::NamedDecl>(m.node)){
        return n->getNameAsString();
        // vs. getQualifiednameAsString() ?
    } else {
        std::cout << "Decl @ " << m.location << "cannot be resolved\n";
        return "INVALID";
    }
}

void printStatistics(std::string text, Matches<clang::Decl> matches){
    std::cout << "\033[33m" << text << "\033[0m " << matches.size() << "\n";
    for(auto m : matches)
        std::cout << getDeclName(m) << " @ " << m.location << std::endl;
}

std::string getCompleteType(Match<clang::Decl> m){
    if(auto n = dyn_cast<clang::VarDecl>(m.node)){ //vardecl or valuedecl?
        return n->getType().getAsString();
    }
}
// std::string getInnerType(Match<clang::Decl> m){
//     if(auto n = dyn_cast<clang::
//          dyn_cast<ValueDecl>(m.node)->getType().getCanonicalType().getAsString()
// }

std::string getBaseType(Match<clang::Decl> m){
    if(auto n = dyn_cast<clang::ValueDecl>(m.node)){
        return n->getType().getTypePtr()->getLocallyUnqualifiedSingleStepDesugaredType().getAsString();
        // return n->getType().getTypePtr()->getCanonicalTypeInternal().getAsString();


    }
}

// std::string getBaseType2(Match<clang::Decl> m){
//     if(auto n = dyn_cast<clang::VarDecl>(m.node)){
//         return n->getType().getTypePtr()->getBaseTypeIdentifier()->getName().getAsString();
//
//     }
// }

//-----------------------------------------------------------------------------
// How often were constructs from standard library used (like vector, array,
// map, list, unordered_map, set etc.). Were they used directly as type,
// or as part of another constructs? What behavior can we see when they are
// passed around? What sizes do they occur (#elements, constexpr)?
// Usage in templates and TMP?

StdlibAnalysis::StdlibAnalysis(clang::ASTContext& Context) : Analysis(Context) {
}
void StdlibAnalysis::extract() {

    std::array<std::string, 2> ContainerTypes = {"vector", "array"};
    // auto isAnyStdContainer = [](){
    //     return
    // }

    // All variable declarations
    auto Variable = varDecl(
        isExpansionInMainFile(),
        unless(parmVarDecl())
    ).bind("vd");
    auto varDecls = Extr.extract("vd", Variable);
    printStatistics("Contains following variable decls:", varDecls);

    // Variable declarations of std container type
    auto StdContainerVariable = varDecl(
        isExpansionInMainFile(),
        hasType(cxxRecordDecl(
            hasAnyName("vector", "array"),
            isInStdNamespace())))
    .bind("stdcvd");
    auto StdContainerVarDecls = Extr.extract("stdcvd", StdContainerVariable);
    printStatistics("num variable decls", StdContainerVarDecls);
    for(auto m : StdContainerVarDecls) {
    // need to check that cast does not return NULL
        std::cout << getCompleteType(m) << std::endl;
        // std::cout << getBaseType2(m) << std::endl;

    }
}
void StdlibAnalysis::analyze(){

}
void StdlibAnalysis::run(){
    std::cout << "\033[32mRunning standard library analysis:\033[0m" << std::endl;
    extract();

}

//-----------------------------------------------------------------------------
