#include <iostream>
#include <vector>
#include <unordered_map>

#include "llvm/Support/raw_ostream.h"

#include "cxx-langstat/Analyses/StdlibAnalysis.h"
#include "cxx-langstat/Utils.h"

using namespace clang;
using namespace clang::ast_matchers;

//-----------------------------------------------------------------------------

std::string getCompleteType(const DeclaratorDecl* Node){
    return Node->getType().getAsString();
}
std::string getBaseType(const DeclaratorDecl* Node){
    auto QualType = Node->getType();
    const Type* TypePtr = QualType.getTypePtr();
    if(auto ElaboratedTypePtr = dyn_cast<ElaboratedType>(TypePtr)){
        TypePtr = ElaboratedTypePtr->getNamedType().getTypePtr();
    }
    if(auto t = dyn_cast<TemplateSpecializationType>(TypePtr)){
        std::string res = "";
        llvm::raw_string_ostream OS(res);
        t->getTemplateName().dump(OS);
        return res;
    } else {
        return "fail";
    }
}
// http://llvm.org/doxygen/classllvm_1_1raw__string__ostream.html
// Inspiration to use dump to stringify: http://clang.llvm.org/doxygen/
// classclang_1_1TemplateArgument.html#af831654c11a86c68ec44950e1575b9c6
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
        auto numTargs = t->getNumArgs();
        std::string res = "";
        res.append("<");
        for(unsigned idx=0; idx<numTargs; idx++){
            if(idx)
                res.append(",");
            auto Targ = t->getArg(idx);
            llvm::raw_string_ostream OS(res);
            Targ.dump(OS);
        }
        res.append(">");
        return res;
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

void StdlibAnalysis::extractFeatures() {
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
        "queue", "priority_queue", "stack", "deque",
        "pair", "tuple",
        "unique_ptr", "shared_ptr", "weak_ptr"
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

    VarDecls = Extractor.extract(*Context, "vd", Variable);
    StdContainerVarDecls = Extractor.extract(*Context, "stdcvd",
        StdContainerVariable);
    StdContainerFieldDecls = Extractor.extract(*Context, "stdcfd",
        StdContainerField);
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
        if(auto Node = dyn_cast<T>(m.Node)){
            auto CompleteType = getCompleteType(Node);
            auto BaseType = getBaseType(Node);
            auto InnerType = getInnerType(Node);
            if(ContainerOccurrences.count(BaseType) == 0)
                ContainerOccurrences[BaseType] = 1;
            else
                ContainerOccurrences[BaseType]++;
            ContainerOfTypes.insert({BaseType, InnerType});
            // std::cout << m.Location << std::endl;
            // std::cout << Node->getType().getTypePtr()->getTypeClassName() << std::endl;
            // std::cout << Node->getType().getTypePtr()->getLocallyUnqualifiedSingleStepDesugaredType().getAsString() << std::endl;
            // std::cout << CompleteType << " is " << BaseType << " of " << InnerType << std::endl;
            // std::cout << "--------" << std::endl;
        }
    }
    for(auto [key, val] : ContainerOccurrences){
        std::cout << key << "(" << val << "): ";
        auto range = ContainerOfTypes.equal_range(key);
        for (auto it = range.first; it != range.second; it++){
            if(it!=range.first)
                std::cout << ", ";
            std::cout << it->second;
        }
        std::cout << std::endl;
    }
}
void StdlibAnalysis::analyzeFeatures(){
    extractFeatures();
    printStats<VarDecl>("Variable decls of standard library type:",
        StdContainerVarDecls);
    printStats<FieldDecl>("Field decls of standard library type:",
        StdContainerFieldDecls);
}
void StdlibAnalysis::processFeatures(nlohmann::ordered_json j){
    
}

//-----------------------------------------------------------------------------
