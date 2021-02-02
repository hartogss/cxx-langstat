#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <string>

#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/raw_os_ostream.h"
#include "llvm/ADT/StringRef.h"
#include "clang/AST/TemplateBase.h"

#include <nlohmann/json.hpp>

#include "cxx-langstat/Analyses/TemplateInstantiationAnalysis.h"
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
    TemplateInstantiationAnalysis(false, anything()) {
}

internal::VariadicDynCastAllOfMatcher<Decl, VarTemplateDecl> varTemplateDecl;

// Can restrict TIA with hasName or hasAnyName matcher to only look for instant-
// iations of certain class templates
TemplateInstantiationAnalysis::TemplateInstantiationAnalysis(
    bool analyzeClassInstsOnly, internal::Matcher<clang::NamedDecl> Names) :
    analyzeClassInstsOnly(analyzeClassInstsOnly),
    ClassInstMatcher(
        decl(anyOf(
            // -- Implicit --
            // Implicit uses:
            // Variable declarations (which include function parameter variables
            // & static member variables)
            varDecl(
                isExpansionInMainFile(),
                // Want variable that has type of some class instantiation,
                // class name is restricted to come from 'Names'
                hasType(
                    classTemplateSpecializationDecl(
                        Names,
                        isTemplateInstantiation())
                    .bind("ImplicitCTSD"))
            )
            .bind("VarsFieldThatInstantiateImplicitly"),
            // Field declarations (non static variable member)
            fieldDecl(
                isExpansionInMainFile(),
                hasType(
                    classTemplateSpecializationDecl(
                        Names,
                        isTemplateInstantiation())
                    .bind("ImplicitCTSD")))
            .bind("VarsFieldThatInstantiateImplicitly"),

            // -- Explicit --
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

// test code for instantiations:
// - Test code to see if able to find "subinstantiations"
// classTemplateSpecializationDecl(
//     Names,
//     isTemplateInstantiation(),
//     unless(has(cxxRecordDecl())))
// .bind("ImplicitCTSD"),
// - If variable is inside of a template, then it has to be the
// case that the template is being instantiated
// This doesn't work transitively
// anyOf(unless(hasAncestor(functionTemplateDecl())), isInstantiated()),
// - A variable of CTSD type can be matched by this matcher. For
// some reason if it is, it will match twice. Either you uncomment
// the line down below and disallow those variable instantiations
// to be matched here, or you filter out the duplicates below.
// See VarTemplateInstClass.cpp for insights.
// unless(isTemplateInstantiation())

void TemplateInstantiationAnalysis::extractFeatures() {
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

    // No doubt, this is a dirty fix. If some class type was instantiated through
    // a variable templates instantiation, which will be matched twice, the loop
    // below will filter out those variable declarations and the belonging
    // implicit class inst with it in O(n^2). A better solution would be to
    // bundle variables with the class template specialization that is their type
    // and sort (O(nlogn)), but for sake of easyness this is (still) omitted.
    // That they're matched twice is due to an bug in RecursiveASTVisitor:
    // https://lists.llvm.org/pipermail/cfe-dev/2021-February/067595.html
    // std::cout << ImplicitInsts.size() << std::endl;
    if(!ImplicitInsts.empty()){
        for(int i=1; i<ImplicitInsts.size(); i++){
            for(int j=0; j<i; j++){
                std::cout << i << ", " << j << std::endl;
                if(i!=j && ImplicitInsts.at(i) == ImplicitInsts.at(j)){
                    std::cout << "erased" << std::endl;
                    ImplicitInsts.erase(ImplicitInsts.begin()+i);
                    ClassImplicitInsts.erase(ClassImplicitInsts.begin()+i);
                    i--;
                }
            }
        }
    }

    // In contrast, this result gives a pointer to a functionDecl, which has
    // too has a function we can call to get the template arguments.
    // Here, the location reported is in both explicit and implicit cases
    // the location where the function template is defined
    // (to be precise, the line where the return value is specified).
    auto FuncInstMatcher = functionDecl(
        isExpansionInMainFile(),
        isTemplateInstantiation())
    .bind("FuncInsts");
    if(!analyzeClassInstsOnly){
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
    if(!analyzeClassInstsOnly){
        auto VarResults = Extractor.extract2(*Context, VarInstMatcher);
        VarInsts = getASTNodes<VarTemplateSpecializationDecl>(VarResults,
            "VarInsts");
        if(VarInsts.size())
            removeDuplicateMatches(VarInsts);
    }
}

// Overloaded function to get template arguments depending whether it's a class
// function, or variable.
const TemplateArgumentList*
getTemplateArgs(const Match<ClassTemplateSpecializationDecl>& Match){
    return &(Match.Node->getTemplateInstantiationArgs());
}
const TemplateArgumentList*
getTemplateArgs(const Match<VarTemplateSpecializationDecl>& Match){
    return &(Match.Node->getTemplateInstantiationArgs());
}
// Is this memory-safe?
// Probably yes, assuming the template arguments being stored on the heap,
// being freed only later by the clang library.
const TemplateArgumentList*
getTemplateArgs(const Match<FunctionDecl>& Match){
    // If Match.Node is a non-templated method of a class template
    // we don't care about its instantiation. Then only the class instantiation
    // encompassing it is really interesting, which is output at a different
    // points in code (and time).
    if(auto m = dyn_cast<CXXMethodDecl>(Match.Node)){
        // std::cout << "found method" << std::endl;
        if(m->getInstantiatedFromMemberFunction())
            return nullptr;
    }
    auto TALPtr = Match.Node->getTemplateSpecializationArgs();
    if(!TALPtr){
        std::cerr << "Template argument list ptr is nullptr,"
        << " function declaration at line " << Match.Location
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
unsigned TemplateInstantiationAnalysis::getInstantiationLocation(
    const Match<ClassTemplateSpecializationDecl>& Match, bool isImplicit){
    SourceManager& SM = Context->getSourceManager();
    if(isImplicit){
        ImplicitInstCounter++;
        auto FSL = FullSourceLoc(ImplicitInsts[ImplicitInstCounter-1].Node->getInnerLocStart(), SM);
        return FSL.getLineNumber();
        // can't I just do ImplicitInsts[i-1].Location to get loc of var/field?
        // std::cout << ImplicitInsts[i-1].Location << std::endl;
        // return std::to_string(static_cast<int>(ImplicitInsts[i-1].Location));
    } else{
        auto FSL = FullSourceLoc(Match.Node->getTemplateKeywordLoc(), SM);
        return FSL.getLineNumber();
        // when giving location of explicit inst, can just give match.Location,
        // since CTSD holds right location already since not subtree of CTD
        // return Match.Location;
    }
}
template<typename T>
unsigned TemplateInstantiationAnalysis::getInstantiationLocation(
    const Match<T>& Match, bool imp){
        SourceManager& SM = Context->getSourceManager();
        auto FSL = FullSourceLoc(Match.Node->getPointOfInstantiation(), SM);
        return FSL.getLineNumber();
}

// Given a vector of matches, create a JSON object storing all instantiations.
template<typename T>
void TemplateInstantiationAnalysis::gatherInstantiationData(Matches<T>& Insts,
    std::string InstKind, bool AreImplicit){
    const std::array<std::string, 3> ArgKinds = {"non-type", "type", "template"};
    ordered_json instances;
    for(auto match : Insts){
        // std::cout << getMatchDeclName(match) << ":" << match.Node->getSpecializationKind() << std::endl;
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
            // instance["location"] = match.Location;
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
    Features[InstKind] = instances;
}

void TemplateInstantiationAnalysis::analyzeFeatures(){
    extractFeatures();
    llvm::raw_os_ostream stream2(std::cout);
    gatherInstantiationData(ClassExplicitInsts, "explicit class insts", false);
    gatherInstantiationData(ClassImplicitInsts, "implicit class insts", true);
    if(!analyzeClassInstsOnly)
        gatherInstantiationData(FuncInsts, "func insts", false);
    std::cout << std::endl;
    if(!analyzeClassInstsOnly)
        gatherInstantiationData(VarInsts, "var insts", false);
}
void TemplateInstantiationAnalysis::processFeatures(nlohmann::ordered_json j){

}

void TemplateInstantiationAnalysis::ResetAnalysis(){
    ClassImplicitInsts.clear();
    ClassExplicitInsts.clear();
    ImplicitInsts.clear();
    FuncInsts.clear();
    VarInsts.clear();
    ImplicitInstCounter=0;
}

//-----------------------------------------------------------------------------
