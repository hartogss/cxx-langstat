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
using json = nlohmann::json;
using ordered_json = nlohmann::ordered_json;
using StringRef = llvm::StringRef;

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
// - classes:
//   * report every explicit instantiation
//   * report every "implicit instantiation", e.g. report ALL instances of
//     std::vector<int>, notably when that occurs multiple times
// - functions:
//   * report every call of f<t> for some instantiating arguments.
//   * explicit function templates are not reported for brevity and due to time,
//     and ease of implementation
// - variables: Report for each var<t> the instantiation, no matter how often
//   that happened.

TemplateInstantiationAnalysis::TemplateInstantiationAnalysis() :
    TemplateInstantiationAnalysis(InstKind::Any, anything(), ".*") {
}

TemplateInstantiationAnalysis::TemplateInstantiationAnalysis(InstKind IK,
    internal::Matcher<clang::NamedDecl> Names, std::string HeaderRegex) :
    IK(IK), Names(Names), HeaderRegex(HeaderRegex) {
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
    if(IK == InstKind::Class || IK == InstKind::Any){

        // Want variable that has type of some class instantiation,
        // class name is restricted to come from 'Names'
        // WIP: supporting references/pointer to CTSD too.
        auto CTSD = classTemplateSpecializationDecl(
            Names,
            isTemplateInstantiation(),
            isExpansionInFileMatching(HeaderRegex))
            .bind("ImplicitCTSD");
        auto type = hasCanonicalType(hasDeclaration(CTSD));
        // Alternative?
        // hasUnqualifiedDesugaredType(recordType(hasDeclaration(CTSD))) 
        auto typematcher = //anyOf(
            hasType(type)/*,
            hasType(references(type)),
            hasType(pointsTo(type)))*/;

        auto ClassInstMatcher = decl(anyOf(
            // -- Implicit --
            // Implicit uses:
            // Variable declarations (which include function parameter variables
            // & static member variables)
            varDecl(
                isExpansionInMainFile(),
                typematcher
                )
            .bind("VarsFieldThatInstantiateImplicitly"),
            // Field declarations (non static variable member)
            fieldDecl(
                isExpansionInMainFile(),
                typematcher)
            .bind("VarsFieldThatInstantiateImplicitly"),

            // -- Explicit --
            // Explicit instantiations that are not explicit specializations,
            // which is ensured by isTemplateInstantiation() according to
            // matcher reference
            classTemplateSpecializationDecl(
                Names,
                isExpansionInMainFile(), // FIXME: either this or isExpansionInFileMatching with anyOf?
                // should not be stored where classtemplate is stored,
                // because there the implicit instantiations are usually put
                unless(hasParent(classTemplateDecl())),
                isTemplateInstantiation())
            .bind("ExplicitCTSD")));
        auto ClassResults = Extractor.extract2(*Context, ClassInstMatcher);
        ClassExplicitInsts = getASTNodes<ClassTemplateSpecializationDecl>(ClassResults,
            "ExplicitCTSD");
        ClassImplicitInsts = getASTNodes<ClassTemplateSpecializationDecl>(ClassResults,
            "ImplicitCTSD");
        // only really needed to find location of where class was implicitly instantiated
        // using variable of member variable/field
        Variables = getASTNodes<DeclaratorDecl>(ClassResults,
            "VarsFieldThatInstantiateImplicitly");

        // No doubt, this is a botch. If some class type was instantiated through
        // a variable templates instantiation, which will be matched twice, the loop
        // below will filter out those variable declarations and the belonging
        // implicit class inst with it in O(n^2). A better solution would be to
        // bundle variables with the class template specialization that is their type
        // and sort (O(nlogn)), but for sake of easyness this is (still) omitted.
        // That they're matched twice is due to an bug in RecursiveASTVisitor:
        // https://lists.llvm.org/pipermail/cfe-dev/2021-February/067595.html
        // std::cout << Variables.size() << std::endl;
        if(!Variables.empty()){
            for(int i=1; i<Variables.size(); i++){
                for(int j=0; j<i; j++){
                    if(i!=j && Variables.at(i) == Variables.at(j)){
                        Variables.erase(Variables.begin()+i);
                        ClassImplicitInsts.erase(ClassImplicitInsts.begin()+i);
                        i--;
                    }
                }
            }
        }
    }

    //
    if(IK == InstKind::Function || IK == InstKind::Any){
        // Old code that matched instantiations no matter explicit/implicit.
        // auto FuncInstMatcher = functionDecl(
        //     isExpansionInMainFile(),
        //     Names,
        //     isTemplateInstantiation())
        // .bind("FuncInsts");
        // Capture all calls that potentially cause an instantiation of a
        // function template. Explicit function template instantation are ignored
        // for easity.
        auto FuncInstMatcher = callExpr(callee(
            functionDecl(
                Names,
                isTemplateInstantiation(),
                isExpansionInFileMatching(HeaderRegex)
            ).bind("FuncInsts")),
            isExpansionInMainFile()
        ).bind("callers");
        auto FuncResults = Extractor.extract2(*Context, FuncInstMatcher);
        FuncInsts = getASTNodes<FunctionDecl>(FuncResults, "FuncInsts");
        Callers = getASTNodes<CallExpr>(FuncResults, "callers");
        // for(int i=0; i<FuncInsts.size(); i++){
        //     std::cout << getMatchDeclName(FuncInsts[i]) << " @ " << Callers[i].Location << std::endl;
        // }
    }

    // Same behavior as with classTemplates: gives pointer to a
    // varSpecializationDecl. However, the location reported is that of the
    // varDecl itself... no matter if explicit or implicit instantiation.
    if(IK == InstKind::Variable || IK == InstKind::Any){
        internal::VariadicDynCastAllOfMatcher<Decl, VarTemplateSpecializationDecl>
            varTemplateSpecializationDecl;
        auto VarInstMatcher = varTemplateSpecializationDecl(
            isExpansionInMainFile(),
            isTemplateInstantiation())
        .bind("VarInsts");
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
        if(m->getInstantiatedFromMemberFunction())
            return nullptr;
    }
    auto TALPtr = Match.Node->getTemplateSpecializationArgs();
    if(!TALPtr){
        std::cerr << "Template argument list ptr is nullptr,"
        << " function declaration " << getMatchDeclName(Match) << " at line " << Match.Location
        << " was not a template specialization" << '\n';
        // should we exit here, or just continue and ignore this failure?
        // did not occur often in my testing so far.
        // happened once when running TIA on Driver.cpp
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
    if(isImplicit){
        return Context->getFullLoc(Variables[VariablesCounter++].Node->getInnerLocStart())
            .getLineNumber();
        // can't I just do Variables[i-1].Location to get loc of var/field?
        // std::cout << Variables[i-1].Location << std::endl;
        // return std::to_string(static_cast<int>(Variables[i-1].Location));
    } else{
        return Context->getFullLoc(Match.Node->getTemplateKeywordLoc())
            .getLineNumber();
        // when giving location of explicit inst, can just give match.Location,
        // since CTSD holds right location already since not subtree of CTD
        // return Match.Location;
    }
}

unsigned TemplateInstantiationAnalysis::getInstantiationLocation(
    const Match<FunctionDecl>& Match, bool isImplicit){
        if(isImplicit){
            // std::cout << CallersCounter << ", " << Callers[CallersCounter].Location << std::endl;
            return Callers[CallersCounter++].Location;
        } else {
            return Context->getFullLoc(Match.Node->getPointOfInstantiation())
                .getLineNumber();
        }
}

template<typename T>
unsigned TemplateInstantiationAnalysis::getInstantiationLocation(
    const Match<T>& Match, bool imp){
        return Context->getFullLoc(Match.Node->getPointOfInstantiation())
            .getLineNumber();
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
            // FIXME: find more elegant solution
            // No TAL -> skip a function call in reporting -> increase counter
            // to get correct call for each object in FuncInst
            if(InstKind == "func insts")
                CallersCounter++;
        }
    }
    Features[InstKind] = instances;
}

void TemplateInstantiationAnalysis::analyzeFeatures(){
    extractFeatures();
    if(IK == InstKind::Class || IK == InstKind::Any){
        gatherInstantiationData(ClassExplicitInsts, "explicit class insts", false);
        gatherInstantiationData(ClassImplicitInsts, "implicit class insts", true);
    }
    if(IK == InstKind::Function || IK == InstKind::Any)
        gatherInstantiationData(FuncInsts, "func insts", true);
    if(IK == InstKind::Variable || IK == InstKind::Any)
        gatherInstantiationData(VarInsts, "var insts", false);
}

void TemplateInstantiationAnalysis::processFeatures(nlohmann::ordered_json j){
    if(j.contains("explicit class insts")) {
        ordered_json res;
        typePrevalence(j.at("explicit class insts"), res);
        Statistics["tia explicit class insts"] = res;
    }
    if(j.contains("implicit class insts")) {
        ordered_json res;
        typePrevalence(j.at("implicit class insts"), res);
        Statistics["tia implicit class insts"] = res;
    }
    if(j.contains("func insts")) {
        ordered_json res;
        typePrevalence(j.at("func insts"), res);
        Statistics["func insts"] = res;
    }

}

void TemplateInstantiationAnalysis::ResetAnalysis(){
    ClassImplicitInsts.clear();
    ClassExplicitInsts.clear();
    Variables.clear();
    FuncInsts.clear();
    Callers.clear();
    VarInsts.clear();
    VariablesCounter=0;
    CallersCounter=0;
}

int getNumRelevantTypes(StringRef Type, const StringMap<int>& SM){
    if(auto [l,r] = Type.split("::__1"); !r.empty())
        return SM.at((l+r).str());
    return SM.at(Type.str());
}

std::string getRelevantTypesAsString(StringRef Type, json Types,
    const StringMap<int>& SM){
        // std::cout << ContainerType.str() << std::endl;
        int n = getNumRelevantTypes(Type, SM);
        if(n == -1)
            n = Types.end()-Types.begin();
        std::string t;
        for(nlohmann::json::iterator i=Types.begin(); i<Types.begin()+n; i++)
            t = t + (*i).get<std::string>() + ", ";
        if(n)
            return llvm::StringRef(t).drop_back(2).str();
        else
            return "";
}

void typePrevalence(const ordered_json& in, ordered_json& out){
    std::map<std::string, unsigned> m;
    for(const auto& [Type, Insts] : in.items()){
        // std::cout << Type << std::endl;
        m.try_emplace(Type, Insts.size());
    }
    out = m;

}

void instantiationTypeArgs(const ordered_json& in, ordered_json& out,
    const StringMap<int>& SM){
        StringMap<StringMap<unsigned>> m;
        for(const auto& [Type, Insts] : in.items()){
            for(const auto& Inst : Insts){
                m.try_emplace(Type, StringMap<unsigned>());
                json ContainedTypes = Inst["arguments"]["type"];
                assert(ContainedTypes.is_array());
                auto TypeString = getRelevantTypesAsString(Type, ContainedTypes, SM);
                // std::cout << TypeString << std::endl;
                if(!TypeString.empty()){
                    m.at(Type).try_emplace(TypeString, 0);
                    m.at(Type).at(TypeString)++;
                }
            }
        }
        out = m;
}

//-----------------------------------------------------------------------------
