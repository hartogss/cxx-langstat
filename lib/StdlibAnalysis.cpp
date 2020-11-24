#include <iostream>
#include <vector>
#include <unordered_map>

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
void printDecls(std::string text, Matches<clang::Decl> matches){
    std::cout << "\033[33m" << text << "\033[0m " << matches.size() << "\n";
    for(auto m : matches)
        std::cout << getDeclName(m) << " @ " << m.location << std::endl;
}
std::string getCompleteType(const DeclaratorDecl* Node){
    return Node->getType().getAsString();
}
std::string getBaseType(const DeclaratorDecl* Node){
    if(auto BTI = Node->getType().getBaseTypeIdentifier())
        return BTI->getName().str();
    else
        return "unknown";
}
std::string getInnerType(const DeclaratorDecl* Node){
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
        const TemplateArgument* Targs = t->getArgs();
        auto QualType = Targs->getAsType();
        return QualType->getLocallyUnqualifiedSingleStepDesugaredType().getAsString();
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
    // All variable declarations
    DeclarationMatcher Variable = varDecl(
        isExpansionInMainFile(),
        unless(hasAncestor(cxxConstructorDecl(isImplicit())))
    ).bind("vd");
    // Variable declarations of std container type
    auto isAnyStdContainer = hasAnyName( // Copied from UseAutoCheck.cpp
        "array", "vector",
        "forward_list", "list",
        "map", "multimap",
        "set", "multiset",
        "unordered_map", "unordered_multimap",
        "unordered_set", "unordered_multiset",
        "queue", "priority_queue", "stack", "deque"
        );
    auto requirements = hasType(namedDecl( // previously cxxRecordDecl
            isAnyStdContainer,
            isInStdNamespace()));
    DeclarationMatcher StdContainerVariable = decl(
        isExpansionInMainFile(),
        varDecl(requirements))
    .bind("stdcvd");
    DeclarationMatcher StdContainerField = decl(
        isExpansionInMainFile(),
        fieldDecl(requirements))
    .bind("stdcfd");

    VarDecls = Extr.extract("vd", Variable);
    StdContainerVarDecls = Extr.extract("stdcvd", StdContainerVariable);
    StdContainerFieldDecls = Extr.extract("stdcfd", StdContainerField);
    // printDecls("Contains following variable decls:", VarDecls);
    // printDecls("Variable decls of standard library type:", StdContainerVarDecls);
    // printDecls("Field decls of standard library type:", StdContainerFieldDecls);
}
template<typename T>
void printStats(std::string text, Matches<Decl> Matches){
    std::cout << "\033[33m" << text << "\033[0m " << Matches.size() << "\n";
    std::unordered_map<std::string, unsigned> ContainerOccurrences;
    std::unordered_multimap<std::string, std::string> ContainerOfTypes;
    for(auto m : Matches) {
        if(auto Node = dyn_cast<T>(m.node)){
            auto CompleteType = getCompleteType(Node);
            auto BaseType = getBaseType(Node);
            auto InnerType = getInnerType(Node);
            if(ContainerOccurrences.count(BaseType) == 0)
                ContainerOccurrences[BaseType] = 1;
            else
                ContainerOccurrences[BaseType]++;
            ContainerOfTypes.insert({BaseType, InnerType});
            // std::cout << m.location << std::endl;
            // std::cout << Node->getType().getTypePtr()->getTypeClassName() << std::endl;
            // std::cout << Node->getType().getTypePtr()->getLocallyUnqualifiedSingleStepDesugaredType().getAsString() << std::endl;
            // std::cout << CompleteType << " is " << BaseType << " of " << InnerType << std::endl;
            // std::cout << "--------" << std::endl;
        }
    }
    for(auto [key, val] : ContainerOccurrences){
        std::cout << key << "(" << val << "): ";
        auto range = ContainerOfTypes.equal_range(key);
        for (auto it = range.first; it != range.second; it++)
            std::cout << it->second << ", ";
        std::cout << std::endl;
    }
}
void StdlibAnalysis::analyze(){
    printStats<VarDecl>("Variable decls of standard library type:",
        StdContainerVarDecls);
    printStats<FieldDecl>("Field decls of standard library type:",
        StdContainerFieldDecls);
}
void StdlibAnalysis::run(){
    std::cout << "\033[32mRunning standard library analysis:\033[0m" << std::endl;
    extract();
    analyze();
}

//-----------------------------------------------------------------------------

// std::cout << Node->getType().getTypePtr()->isDependentType() << std::endl;
// std::cout << Node->getType().getTypePtr()->hasIntegerRepresentation() << std::endl;
// std::cout << Node->getType().getTypePtr()->isInstantiationDependentType() << std::endl;
// std::cout << Node->getType().getCanonicalType().getAsString() << std::endl;
// std::cout << Node->getType().getTypePtr()->getCanonicalTypeInternal().getAsString() << std::endl; // gives the same as line above, but without qualifiers since stripped away

// std::array<std::string, 2> ContainerTypes = {"vector", "array"};
// auto isAnyStdContainer = [](auto ContainerTypes){
//     return hasAnyName()
// };

// const char* StmtKind = m.node->getDeclKindName();

// TypeMatcher stdtypes = type().bind("t");
// auto t = Extr.extract("t", stdtypes);
// for(auto match : t){
//     std::cout << match.node->getAsString()
//         << " @ " << match.location << std::endl;
// }