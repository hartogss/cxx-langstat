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
TemplateInstantiationAnalysis::TemplateInstantiationAnalysis() :
    TemplateInstantiationAnalysis(anything()) {
}

internal::VariadicDynCastAllOfMatcher<Decl, VarTemplateDecl> varTemplateDecl;

// Can restrict TIA with hasName or hasAnyName matcher to only look for instant-
// iations of certain class templates
TemplateInstantiationAnalysis::TemplateInstantiationAnalysis(
    internal::Matcher<clang::NamedDecl> Names) :
    ClassInstMatcher(
        decl(
            anyOf(
            // Implicit uses:
            // Variable declarations (which include function parameter variables
            // & static fields)
            varDecl(
                isExpansionInMainFile(),
                // unless(hasParent(varTemplateDecl())), // this seems superfluous
                // If variable is inside of a template, then it has to be the
                // case that the template is being instantiated
                // anyOf(unless(hasAncestor(functionTemplateDecl())), isInstantiated()),
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
                // Count fielddecls only if inside template instantiation
                // anyOf(unless(hasAncestor(classTemplateDecl())), isInstantiated()),
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
        std::cout << "TIA ctor\n";
}

void TemplateInstantiationAnalysis::extract() {
    // Result of the class insts matcher will give back a pointer to the
    // ClassTemplateSpecializationDecl (CTSD).
    // Matcher constructed by ctor
    auto ClassResults = Extractor.extract2(*Context, ClassInstMatcher);
    ClassExplicitInsts = getASTNodes<ClassTemplateSpecializationDecl>(ClassResults,
        "ExplicitCTSD");
    ClassImplicitInsts = getASTNodes<ClassTemplateSpecializationDecl>(ClassResults,
        "ImplicitCTSD");
    // only really needed to find location of where class was implicitly instantiated
    // using variable of member variable/field
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
        auto FuncResults = Extractor.extract2(*Context, FuncInstMatcher);
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
        auto VarResults = Extractor.extract2(*Context, VarInstMatcher);
        VarInsts = getASTNodes<VarTemplateSpecializationDecl>(VarResults,
            "VarInsts");
        if(VarInsts.size())
            removeDuplicateMatches(VarInsts);
        // printMatches("var", VarInsts);
    }
}

// Overloaded function to get template arguments depending whether it's a class
// function, or variable.
const TemplateArgumentList*
getTemplateArgs(const Match<ClassTemplateSpecializationDecl>& Match){
    return &(Match.node->getTemplateInstantiationArgs());
}
const TemplateArgumentList*
getTemplateArgs(const Match<VarTemplateSpecializationDecl>& Match){
    return &(Match.node->getTemplateInstantiationArgs());
}
// Is this memory-safe?
// Probably yes, assuming the template arguments being stored on the heap,
// being freed only later by the clang library.
const TemplateArgumentList*
getTemplateArgs(const Match<FunctionDecl>& Match){
    // If Match.node is a non-templated method of a class template
    // we don't care about its instantiation. Then only the class instantiation
    // encompassing it is really interesting, which is output at a different
    // points in code (and time).
    if(auto m = dyn_cast<CXXMethodDecl>(Match.node)){
        // std::cout << "found method" << std::endl;
        if(m->getInstantiatedFromMemberFunction())
            return nullptr;
    }
    auto TALPtr = Match.node->getTemplateSpecializationArgs();
    if(!TALPtr){
        std::cerr << "Template argument list ptr is nullptr,"
        << " function declaration at line " << Match.location
        << " was not a template specialization" << '\n';
        exit(1);
    }
    return TALPtr;
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
            printToString(Context->getSourceManager());
    } else{
        return Match.node->getTemplateKeywordLoc().
            printToString(Context->getSourceManager());
    }
}
template<typename T>
std::string TemplateInstantiationAnalysis::getInstantiationLocation(
    const Match<T>& Match, bool imp){
        return Match.node->getPointOfInstantiation().
            printToString(Context->getSourceManager());
}

// Given a vector of matches, create a JSON object storing all instantiations.
template<typename T>
void TemplateInstantiationAnalysis::gatherStatistics(Matches<T>& Insts,
    std::string InstKind, bool AreImplicit){
    const std::array<std::string, 3> ArgKinds = {"non-type", "type", "template"};
    ordered_json instances;
    for(auto match : Insts){
        // std::cout << getMatchDeclName(match) << std::endl;
        std::multimap<std::string, std::string> TArgs;
        const TemplateArgumentList* TALPtr(getTemplateArgs(match));
        // Only report instantiation if it had any arguments it was instantiated
        // with.
        if(TALPtr){
            auto numTArgs = TALPtr->size();
            for(unsigned idx=0; idx<numTArgs; idx++){
                auto TArg = TALPtr->get(idx);
                updateArgsAndKinds(TArg, TArgs);
            }
            ordered_json instance;
            ordered_json arguments;
            instance["location"] = getInstantiationLocation(match, AreImplicit);
            // instance["location"] = match.location;
            for(auto key : ArgKinds){
                auto range = TArgs.equal_range(key);
                std::vector<std::string> v;
                for (auto it = range.first; it != range.second; it++)
                    v.emplace_back(it->second);
                arguments[key] = v;
            }
            instance["arguments"] = arguments;
            // Use emplace instead of '=' because can be mult. insts for a template
            instances[getMatchDeclName(match)].emplace_back(instance);
        } else {
            std::cout << getMatchDeclName(match) << " had no inst args\n";
        }
    }
    Result[InstKind] = instances;
}

void TemplateInstantiationAnalysis::run(llvm::StringRef InFile,
        ASTContext& Context){
        std::cout << "\033[32mRunning template instantiation analysis:\033[0m\n";
        this->Context = &Context;
        extract();
        llvm::raw_os_ostream stream2(std::cout);
        gatherStatistics(ClassExplicitInsts, "explicit class insts", false);
        gatherStatistics(ClassImplicitInsts, "implicit class insts", true);
        if(analyzeFuncInsts)
            gatherStatistics(FuncInsts, "func insts", false);
        if(analyzeVarInsts)
            gatherStatistics(VarInsts, "var insts", false);
}

//-----------------------------------------------------------------------------
