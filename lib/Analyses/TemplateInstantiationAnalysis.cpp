#include <iostream>
#include <vector>

#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/raw_os_ostream.h"
#include "llvm/ADT/StringRef.h"
#include "clang/AST/TemplateBase.h"

#include "cxx-langstat/Analyses/TemplateInstantiationAnalysis.h"
#include "cxx-langstat/Stats.h"
#include "cxx-langstat/Utils.h"

using namespace clang;
using namespace clang::ast_matchers;


//-----------------------------------------------------------------------------
// Compute statistics on arguments used to instantiate templates,
// no matter whether instantiation are explicit or implicit.
// Should be divided into 3 categories:
// - classes,
// - functions (including member methods), &
// - variables (including class static member variables, (not
// class field, those cannot be templated if they're not static))
// Template instantiations counted should stem from either explicit instantiations
// written by programmers or from implicit ones through 'natural usage'.
//
// Remember that template parameters can be non-types, types or templates.
// Goal: for each class, function, variable, report:
// - Number of instantiations
// - for individually non-types, types, templates parameters report how often
// they occured.
// - what those arguments actually were:
//      - non-types: what types did those parameters have, possible range of values used?
//      - types: what types were templates instantiated with?
//      - templates: names of template used.

TemplateInstantiationAnalysis::TemplateInstantiationAnalysis
(clang::ASTContext& Context) : Analysis(Context) {
}
void TemplateInstantiationAnalysis::extract() {
    // Oddly enough, the result of this matcher will give back a pointer to the
    // ClassTemplateSpecializationDecl containing the instantiated CXXRecordDecl,
    // not a pointer to the cxxRecordDecl. This is why a cast to
    // ClassTemplateSpecializationDecl is OK.
    // Note that because it returns ClassTemplateSpecializationDecl*, the
    // location reported here is wrong for implicit instantiations because they
    // are subtrees of the CTSD. Explicit instantiation locations are reported
    // correctly.
    auto ClassInstMatcher = cxxRecordDecl(
        isExpansionInMainFile(),
        isTemplateInstantiation())
    .bind("ClassInsts");
    auto ClassResults = Extr.extract2(ClassInstMatcher);
    ClassInsts = getASTNodes<ClassTemplateSpecializationDecl>(ClassResults,
        "ClassInsts");

    // In contrast, this result gives a pointer to a functionDecl, which has
    // too has a function we can call to get the template arguments.
    // Here, the location reported is in both explicit and implicit cases wrong,
    // the location where the function template is defined is returned
    // (to be precise, the line where the return value is specified).
    auto FuncInstMatcher = functionDecl(
        isExpansionInMainFile(),
        isTemplateInstantiation())
    .bind("FuncInsts");
    auto FuncResults = Extr.extract2(FuncInstMatcher);
    FuncInsts = getASTNodes<FunctionDecl>(FuncResults, "FuncInsts");

    // Same behavior as with classTemplates: gives pointer to a
    // varSpecializationDecl. However, the location reported is that of the
    // varDecl itself... no matter if explicit or implicit instantiation.
    auto VarInstMatcher = varDecl(
        isExpansionInMainFile(),
        isTemplateInstantiation())
    .bind("VarInsts");
    auto VarResults = Extr.extract2(VarInstMatcher);
    VarInsts = getASTNodes<VarTemplateSpecializationDecl>(VarResults, "VarInsts");
}

template<typename T>
const TemplateArgumentList& getTemplateArgs(const Match<T>& Match);
template<>
const TemplateArgumentList&
getTemplateArgs(const Match<ClassTemplateSpecializationDecl>& Match){
    const TemplateArgumentList& TAList(
        Match.node->getTemplateInstantiationArgs());
    return TAList;
}
template<>
const TemplateArgumentList&
getTemplateArgs(const Match<VarTemplateSpecializationDecl>& Match){
    const TemplateArgumentList& TAList(
        Match.node->getTemplateInstantiationArgs());
    return TAList;
}
template<>
const TemplateArgumentList&
getTemplateArgs(const Match<FunctionDecl>& Match){
    auto TALPtr = Match.node->getTemplateSpecializationArgs();
    return *TALPtr;
}

void updateArgKinds(const TemplateArgument& TArg,
    std::map<std::string, unsigned>& Mapping){
        switch (TArg.getKind()){
            case TemplateArgument::Null:
            case TemplateArgument::NullPtr:
                break;
            case TemplateArgument::Type:
                Mapping["type"]++;
                break;
            case TemplateArgument::Declaration:
            case TemplateArgument::Integral:
                Mapping["nontype"]++;
                break;
            case TemplateArgument::Template:
                Mapping["template"]++;
                break;
            case TemplateArgument::Pack:
                for(auto it=TArg.pack_begin(); it!=TArg.pack_end(); it++)
                    updateArgKinds(*it, Mapping);
                break;
            // in case that it is a pack, can that be a pack of templates?
            // parameter pack can be of anything
        }
}

template<typename TemplateInstType>
void printStats(std::string text, const Matches<TemplateInstType>& Insts){
    std::cout << "\033[33m" << text << "\033[0m " << Insts.size() << "\n";
    std::map<std::string, unsigned> ArgKinds {
        {"nontype", 0}, {"type", 0}, {"template", 0},
    };
    for(auto match : Insts){
        std::cout << getMatchDeclName(match) << " @ " << match.location << "\n";
        // https://stackoverflow.com/questions/44397953/retrieve-template-
        // parameters-from-cxxconstructexpr-in-clang-ast
        // Check why it works like this and the other does not
        const TemplateArgumentList& TAList(getTemplateArgs(match));
        // auto TAList = Node->getTemplateInstantiationArgs();
        for(unsigned idx=0; idx<TAList.size(); idx++){
            std::string res;
            auto TArg = TAList.get(idx);
            llvm::raw_string_ostream OS(res);
            TArg.dump(OS);
            updateArgKinds(TArg, ArgKinds);
        }
    }
    for(auto [key, val] : ArgKinds)
        std::cout << key << ": " << val << "\n";
}

void getArgAndKindAsStrings(const TemplateArgument& TArg,
    std::multimap<std::string, std::string>& TArgs) {
        std::string Result;
        llvm::raw_string_ostream stream(Result);
        switch (TArg.getKind()){
            case TemplateArgument::Type:
                TArg.dump(stream);
                TArgs.emplace("type", Result);
                break;
            case TemplateArgument::Expression:
                std::cout << "expr";
                std::cout << std::endl;
                break;
            case TemplateArgument::Null:
            case TemplateArgument::NullPtr:
            case TemplateArgument::Declaration:
            case TemplateArgument::Integral:
                TArg.dump(stream);
                TArgs.emplace("non-type", Result);
                break;
            case TemplateArgument::Template:
                TArg.dump(stream);
                TArgs.emplace("template", Result);
                break;
            case TemplateArgument::Pack:
                for(auto it=TArg.pack_begin(); it!=TArg.pack_end(); it++)
                    getArgAndKindAsStrings(*it, TArgs);
                break;
            // Still two cases missing
        }
}

template<typename T>
void gatherStats(const Matches<T>& Insts, llvm::raw_ostream&& stream){
    stream << "Templates, #Non-type params, #Type params, #Template params"
        "\n";
    const std::array<std::string, 3> ArgKinds = {"non-type", "type", "template"};
    std::map<std::string, unsigned> ArgKindCounts {
        {"nontype", 0}, {"type", 0}, {"template", 0},
    };
    for(auto match : Insts){
        std::multimap<std::string, std::string> TArgs;
        const TemplateArgumentList& TAList(getTemplateArgs(match));
        auto numTArgs = TAList.size();
        for(unsigned idx=0; idx<numTArgs; idx++){
            auto TArg = TAList.get(idx);
            getArgAndKindAsStrings(TArg, TArgs);
        }
        stream << getMatchDeclName(match);
        for(auto key : ArgKinds)
            stream << "," << TArgs.count(key);
        for(auto key : ArgKinds){
            auto range = TArgs.equal_range(key);
            for (auto it = range.first; it != range.second; it++)
                stream << "," << it->second;
        }
        stream << '\n';
    }
}

void TemplateInstantiationAnalysis::analyze(){
    printStats<ClassTemplateSpecializationDecl>("Class instantiations",
        ClassInsts);
    printStats<FunctionDecl>("Function instantiations",
        FuncInsts);
    printStats<VarTemplateSpecializationDecl>("Variable instantiations",
        VarInsts);

    constexpr StringRef str("test.csv");
    std::error_code EC;
    llvm::raw_fd_ostream stream(str, EC);
    llvm::raw_os_ostream stream2(std::cout);
    gatherStats(ClassInsts, std::move(stream));
    gatherStats(FuncInsts, std::move(stream));
    gatherStats(VarInsts, std::move(stream));
}
void TemplateInstantiationAnalysis::run(){
    std::cout << "\033[32mRunning template instantiation analysis:\033[0m\n";
    extract();
    analyze();
}

//-----------------------------------------------------------------------------
