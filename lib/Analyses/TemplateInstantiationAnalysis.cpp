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
// Goal: for each instantiation report:
// - classes: for each class report with which arguments it was reported
//   with. if a class was instantiated with the same arguments multiple times,
//   report it every time, s.t. we can count them.
// - functions: for each function report with which arguments it was reported
//   with. if a function was instantiated with the same arguments multiple times,
//   report it once only. (boolean statistic only)
// - variables: same as with functions

// Regular TIA doesn't care what name the template had
TemplateInstantiationAnalysis::TemplateInstantiationAnalysis
(llvm::StringRef InFile, ASTContext& Context) :
    TemplateInstantiationAnalysis(InFile, Context, anything()) {
}

internal::VariadicDynCastAllOfMatcher<Decl, VarTemplateDecl> varTemplateDecl;

// Can restrict TIA with hasName or hasAnyName matcher to only look for instant-
// iations of certain class templates
TemplateInstantiationAnalysis::TemplateInstantiationAnalysis
(llvm::StringRef InFile, clang::ASTContext& Context,
    internal::Matcher<clang::NamedDecl> Names) :
    Analysis(InFile, Context),
    ClassInstMatcher(
        decl(
            anyOf(
            // Implicit uses:
            // Variable declarations (which include function parameter variables
            // & static fields)
            varDecl(
                isExpansionInMainFile(),
                // unless(hasParent(varTemplateDecl())), // this seems superfluous
                // Want variable that has type of some class instantiation,
                // class name is restricted to come from 'Names'
                hasType(
                    classTemplateSpecializationDecl(
                        Names,
                        isTemplateInstantiation())
                    .bind("ImplicitCTSD")),
                // We however don't want the variable to be an instantiation,
                // thus filtering out variable templates
                unless(isTemplateInstantiation()))
            .bind("VarsFieldThatInstantiateImplicitly"),
            // Field declarations (non static variable member)
            fieldDecl(
                isExpansionInMainFile(),
                // unless(hasParent(varTemplateDecl())), // this seems superfluous
                hasType(
                    classTemplateSpecializationDecl(
                        Names,
                        isTemplateInstantiation())
                    .bind("ImplicitCTSD")))
            .bind("VarsFieldThatInstantiateImplicitly"),
            // Explicit instantiations that are not explicit specializations,
            // which is ensured by isTemplateInstantiation() according to
            // matcher reference
            classTemplateSpecializationDecl(
                Names,
                isExpansionInMainFile(),
                // should not be stored where classtemplate is stored,
                // because there the implicit instantiations are usually put
                unless(hasParent(classTemplateDecl())),
                isTemplateInstantiation())
            .bind("ExplicitCTSD")))
    ) {
        std::cout << "TIA: standard constructor\n";
}

void TemplateInstantiationAnalysis::extract() {
    // Result of the class insts matcher will give back a pointer to the
    // ClassTemplateSpecializationDecl (CTSD).
    // Matcher constructed by ctor
    auto ClassResults = Extr.extract2(ClassInstMatcher);
    ClassExplicitInsts = getASTNodes<ClassTemplateSpecializationDecl>(ClassResults,
        "ExplicitCTSD");
    ClassImplicitInsts = getASTNodes<ClassTemplateSpecializationDecl>(ClassResults,
        "ImplicitCTSD");
    ImplicitInsts = getASTNodes<DeclaratorDecl>(ClassResults,
        "VarsFieldThatInstantiateImplicitly");
    // printMatches("exp", ClassExplicitInsts);
    // printMatches("imp", ClassImplicitInsts);
    // printMatches("decls", ImplicitInsts);

    // In contrast, this result gives a pointer to a functionDecl, which has
    // too has a function we can call to get the template arguments.
    // Here, the location reported is in both explicit and implicit cases
    // the location where the function template is defined
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
        // printMatches("var", VarInsts);
    }
}

// Overloaded function to get template arguments depending whether it's a class
// function, or variable.
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

// Given a mapping from template argumend kind to actual arguments and a given,
// previously unseen argument, check what kind the argument has and add it
// to the mapping.
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
            // Still two cases missing:
            // case TemplateArgument::TemplateExpansion
            // case TemplateArgument::Expression
        }
}

// Note about the reported locations:
// For explicit instantiations, a 'single' CTSD match in the AST is returned
// which contains info about the correct location.
// For implicit instantiations (i.e. 'natural' usage e.g. through the use
// of variables, fields), the location of the CTSD is also reported. However,
// since that is a subtree of the tree representing the ClassTemplateDecl, we
// have to do some extra work to get the location of where the instantiation
// in the code actually occurred, that is, the line where the programmer wrote
// the variable or the field.
std::string TemplateInstantiationAnalysis::getInstantiationLocation(
    const Match<ClassTemplateSpecializationDecl>& Match, bool isImplicit){
    static int i = 0;
    if(isImplicit){
        i++;
        return (ImplicitInsts[i-1].node->getInnerLocStart()).
            printToString(Context.getSourceManager());
    } else{
        return Match.node->getTemplateKeywordLoc().
            printToString(Context.getSourceManager());
    }
}
template<typename T>
std::string TemplateInstantiationAnalysis::getInstantiationLocation(
    const Match<T>& Match, bool imp){
        return Match.node->getPointOfInstantiation().
            printToString(Context.getSourceManager());
}

// Given a vector of matches, create a JSON object storing all instantiations.
template<typename T>
void TemplateInstantiationAnalysis::gatherStats(Matches<T>& Insts,
    std::string InstKind, bool AreImplicit, std::ofstream&& file){
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
        instance["location"] = getInstantiationLocation(match, AreImplicit);
        for(auto key : ArgKinds){
            auto range = TArgs.equal_range(key);
            std::vector<std::string> v;
            for (auto it = range.first; it != range.second; it++)
                v.emplace_back(it->second);
            arguments[key] = v;
        }
        instance["arguments"] = arguments;
        super[getMatchDeclName(match)] = instance;
        // file << j.dump(4) << '\n';
        file << super.dump(4) << '\n';
    }
}

void TemplateInstantiationAnalysis::analyze(){
    llvm::raw_os_ostream stream2(std::cout);
    std::ofstream o(getFileForStatDump(InFile));
    gatherStats(ClassExplicitInsts, "class", false, std::move(o));
    gatherStats(ClassImplicitInsts, "class", true, std::move(o));
    if(analyzeFuncInsts)
        gatherStats(FuncInsts, "func", false, std::move(o));
    if(analyzeVarInsts)
        gatherStats(VarInsts, "var", false, std::move(o));
}
void TemplateInstantiationAnalysis::run(){
    std::cout << "\033[32mRunning template instantiation analysis:\033[0m\n";
    extract();
    analyze();
}

//-----------------------------------------------------------------------------
