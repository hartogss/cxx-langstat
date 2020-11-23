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

std::string getCompleteType(const VarDecl* Node){
    return Node->getType().getAsString();
}
std::string getBaseType(const VarDecl* Node){
    return Node->getType().getBaseTypeIdentifier()->getName().str();
}
std::string getInnerType(const VarDecl* Node){
    auto QualType = Node->getType();
    // Pointer to underlying unqualified type
    const Type* TypePtr = QualType.getTypePtr();
    // If is elaborated type (i.e., has qualified names (uses some
    // namespace resolution) or explicitly uses keyword 'struct', ) strip
    // that away and get the type without that
    if(auto ElaboratedTypePtr = dyn_cast<ElaboratedType>(TypePtr)){
        TypePtr = ElaboratedTypePtr->getNamedType().getTypePtr();
    }
    // Is it always a templatespecializationtype?
    if(auto t = dyn_cast<TemplateSpecializationType>(TypePtr)){
        return t->getArgs()->getAsType()->getLocallyUnqualifiedSingleStepDesugaredType().getAsString();
    } else {
        return "fail";
    }
}

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
        if (auto Node = dyn_cast<VarDecl>(m.node)){
            std::cout << Node->getType().getTypePtr()->getTypeClassName() << std::endl;
            std::cout << getCompleteType(Node) << std::endl;
            std::cout << getBaseType(Node) << std::endl;
            std::cout << getInnerType(Node) << std::endl;



            std::cout << Node->getType().getTypePtr()->getLocallyUnqualifiedSingleStepDesugaredType().getAsString() << std::endl;
            // std::cout << Node->getType().getTypePtr()->isDependentType() << std::endl;
            // std::cout << Node->getType().getTypePtr()->hasIntegerRepresentation() << std::endl;
            // std::cout << Node->getType().getTypePtr()->isInstantiationDependentType() << std::endl;


            std::cout << "--------" << std::endl;

            // std::cout << Node->getType().getCanonicalType().getAsString() << std::endl;
            // std::cout << Node->getType().getTypePtr()->getCanonicalTypeInternal().getAsString() << std::endl; // gives the same as line above, but without qualifiers since stripped away
        }
    }
    // const char* StmtKind = m.node->getDeclKindName();

}
void StdlibAnalysis::analyze(){

}
void StdlibAnalysis::run(){
    std::cout << "\033[32mRunning standard library analysis:\033[0m" << std::endl;
    extract();

}

//-----------------------------------------------------------------------------
