#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>

#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/raw_os_ostream.h"
#include "llvm/ADT/StringRef.h"
#include "clang/AST/TemplateBase.h"

#include <nlohmann/json.hpp>

#include "cxx-langstat/Analyses/TemplateInstantiationAnalysis.h"
#include "cxx-langstat/Stats.h"
#include "cxx-langstat/Utils.h"

using namespace clang;
using namespace clang::ast_matchers;
using ordered_json = nlohmann::ordered_json;


//-----------------------------------------------------------------------------
// Compute statistics on arguments used to instantiate templates,
// no matter whether instantiation are explicit or implicit.
// Should be divided into 3 categories:
// - classes,
// - functions (including member methods), &
// - variables (including class static member variables, (not
//   class field, those cannot be templated if they're not static))
//
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

// TemplateInstantiationAnalysis::TemplateInstantiationAnalysis
// (llvm::StringRef InFile, clang::ASTContext& Context) :
//     Analysis(InFile, Context),
//     ClassInstMatcher(
//     cxxRecordDecl(
//         isExpansionInMainFile(),
//         isTemplateInstantiation())
//     .bind("ClassInsts")
//     ) {
//         std::cout << "TIA: standard constructor\n";
//     }

TemplateInstantiationAnalysis::TemplateInstantiationAnalysis
(llvm::StringRef InFile, clang::ASTContext& Context) :
    Analysis(InFile, Context),
    ClassInstMatcher(
        decl(
            anyOf(
            // Implicit uses: varDecl, fieldDecl, any other?
            // Return types?
            declaratorDecl(
                isExpansionInMainFile(),
                hasType(
                    classTemplateSpecializationDecl(isTemplateInstantiation())
                    .bind("ClassInsts")))
            .bind("implicitdecl"),
            // Explicit instantiations that are not explicit specializations,
            // which is ensure by isTemplateInstantiation() according to
            // matcher reference
            classTemplateSpecializationDecl(
                isExpansionInMainFile(),
                unless(hasParent(classTemplateDecl())),
                isTemplateInstantiation())
            .bind("ClassInsts")))
    ) {
        std::cout << "TIA: standard constructor\n";
    }

TemplateInstantiationAnalysis::TemplateInstantiationAnalysis
(llvm::StringRef InFile, ASTContext& Context,
    internal::Matcher<clang::NamedDecl> Names) :
        Analysis(InFile, Context),
        ClassInstMatcher(
            cxxRecordDecl(
                isExpansionInSystemHeader(),
                Names,
                isTemplateInstantiation())
            .bind("ClassInsts")
        ) {
            analyzeFuncInsts = false;
            analyzeVarInsts = false;
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

    // Matcher constructed by ctor
    auto ClassResults = Extr.extract2(ClassInstMatcher);
    ClassInsts = getASTNodes<ClassTemplateSpecializationDecl>(ClassResults,
        "ClassInsts");
    ImplicitInsts = getASTNodes<DeclaratorDecl>(ClassResults, "implicitdecl");

    // In contrast, this result gives a pointer to a functionDecl, which has
    // too has a function we can call to get the template arguments.
    // Here, the location reported is in both explicit and implicit cases wrong,
    // the location where the function template is defined is returned
    // (to be precise, the line where the return value is specified).
    auto FuncInstMatcher = functionDecl(
        isExpansionInMainFile(),
        isTemplateInstantiation())
    .bind("FuncInsts");
    if(analyzeFuncInsts){
        auto FuncResults = Extr.extract2(FuncInstMatcher);
        FuncInsts = getASTNodes<FunctionDecl>(FuncResults, "FuncInsts");
    }

    // Same behavior as with classTemplates: gives pointer to a
    // varSpecializationDecl. However, the location reported is that of the
    // varDecl itself... no matter if explicit or implicit instantiation.
    internal::VariadicDynCastAllOfMatcher<Decl, VarTemplateSpecializationDecl>
        varTemplateSpecializationDecl;
    auto VarInstMatcher = varTemplateSpecializationDecl(
        isExpansionInMainFile(),
        isTemplateInstantiation())
    .bind("VarInsts");
    if(analyzeVarInsts){
        auto VarResults = Extr.extract2(VarInstMatcher);
        VarInsts = getASTNodes<VarTemplateSpecializationDecl>(VarResults,
            "VarInsts");
        if(VarInsts.size())
            removeDuplicateMatches(VarInsts);
    }
}

const TemplateArgumentList&
getTemplateArgs(const Match<ClassTemplateSpecializationDecl>& Match){
    return Match.node->getTemplateInstantiationArgs();
}
const TemplateArgumentList&
getTemplateArgs(const Match<VarTemplateSpecializationDecl>& Match){
    return Match.node->getTemplateInstantiationArgs();
}
// Is this memory-safe?
// Probably yes, assuming the template arguments being stored on the heap,
// being freed only later by the clang library.
const TemplateArgumentList&
getTemplateArgs(const Match<FunctionDecl>& Match){
    auto TALPtr = Match.node->getTemplateSpecializationArgs();
    if(TALPtr!=nullptr)
        return *TALPtr;
    else {
        std::cerr << "Template argument list ptr is nullptr,"
            << " function declaration at line " << Match.location
            << " was not a template specialization" << '\n';
        exit(1);
    }
}

void updateArgsAndKinds(const TemplateArgument& TArg,
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
                    updateArgsAndKinds(*it, TArgs);
                break;
            // Still two cases missing
        }
}

template<typename T>
std::string TemplateInstantiationAnalysis::getInstantiationLocation(
    const Match<T> Match){
    static int i = 0;
    switch(Match.node->getSpecializationKind()){
        case TSK_Undeclared:
        case TSK_ExplicitSpecialization:
            return "invalid";
        case TSK_ImplicitInstantiation:
            i++;
            return (ImplicitInsts[i-1].node->getInnerLocStart()).
                printToString(Context.getSourceManager());
        case TSK_ExplicitInstantiationDeclaration:
        case TSK_ExplicitInstantiationDefinition:
            return Match.node->getPointOfInstantiation().
                printToString(Context.getSourceManager());
    }
}

template<typename T>
void TemplateInstantiationAnalysis::gatherStats(Matches<T>& Insts, std::ofstream&& file){
    const std::array<std::string, 3> ArgKinds = {"non-type", "type", "template"};
    for(auto match : Insts){
        std::multimap<std::string, std::string> TArgs;
        const TemplateArgumentList& TAList(getTemplateArgs(match));
        auto numTArgs = TAList.size();
        for(unsigned idx=0; idx<numTArgs; idx++){
            auto TArg = TAList.get(idx);
            updateArgsAndKinds(TArg, TArgs);
        }
        ordered_json super;
        ordered_json instance;
        ordered_json arguments;
        instance["location"] = getInstantiationLocation(match);
        for(auto key : ArgKinds){
            auto range = TArgs.equal_range(key);
            std::vector<std::string> v;
            for (auto it = range.first; it != range.second; it++)
                v.emplace_back(it->second);
            arguments[key] = v;
        }
        instance["arguments"] = arguments;
        instance["specialization kind"] = match.node->getSpecializationKind();
        super[getMatchDeclName(match)] = instance;
        // file << j.dump(4) << '\n';
        file << super.dump(4) << '\n';
    }
}


void TemplateInstantiationAnalysis::analyze(){
    llvm::raw_os_ostream stream2(std::cout);
    std::ofstream o(getFileForStatDump(InFile));
    gatherStats(ClassInsts, std::move(o));
    // if(analyzeFuncInsts)
        // gatherStats(FuncInsts, std::move(o));
    if(analyzeVarInsts)
        gatherStats(VarInsts, std::move(o));
}
void TemplateInstantiationAnalysis::run(){
    std::cout << "\033[32mRunning template instantiation analysis:\033[0m\n";
    extract();
    analyze();
}

//-----------------------------------------------------------------------------
