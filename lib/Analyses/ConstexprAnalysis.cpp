#include <iostream>
#include <algorithm>

#include "cxx-langstat/Analyses/ConstexprAnalysis.h"
#include "cxx-langstat/Utils.h"

using namespace clang;
using namespace clang::ast_matchers;
using json = nlohmann::json;
using ordered_json = nlohmann::ordered_json;

//-----------------------------------------------------------------------------

void ConstexprAnalysis::extractFeatures(){
    //
    internal::VariadicDynCastAllOfMatcher<Decl, DecompositionDecl>
        decompositionDecl;
    auto vr = Extractor.extract2(*Context,
        varDecl(isExpansionInMainFile(),
            // Inherit from VarDecl, but not constexpr-able, so don't count these
            unless(anyOf(parmVarDecl(), decompositionDecl())),
            hasInitializer(anything()))
            // Could add extra requirements here to get better relative usage
            // of constexpr variables, i.e. better comparision of variables
            // that are constexpr vs those that would be eligible to be constexpr
        .bind("v"));
    auto Var = getASTNodes<clang::VarDecl>(vr, "v");
    // This we need because of buggy VarTemplateSpecializationDecl
    if(!Var.empty())
        removeDuplicateMatches(Var);
    for(auto v : Var)
        Variables.emplace_back(CEDecl(v.Location, v.Node->isConstexpr(),
            getMatchDeclName(v), v.Node->getType().getAsString()));
    //
    auto fr = Extractor.extract2(*Context,
        functionDecl(isExpansionInMainFile(),
            // DeductionGuideDecl inherits from FunctionDecl, but not constexpr-able,
            // so don't count here. MethodDecl want to count separately
            unless(anyOf(cxxMethodDecl(), cxxDeductionGuideDecl())))
        .bind("f"));
    auto Func = getASTNodes<clang::FunctionDecl>(fr, "f");
    for(auto v : Func)
        NonMemberFunctions.emplace_back(CEDecl(v.Location, v.Node->isConstexpr(),
            getMatchDeclName(v), v.Node->getType().getAsString()));
    //
    auto mr = Extractor.extract2(*Context,
        cxxMethodDecl(isExpansionInMainFile())
        .bind("m"));
    auto Method = getASTNodes<clang::FunctionDecl>(mr, "m");
    for(auto v : Method)
        MemberFunctions.emplace_back(CEDecl(v.Location, v.Node->isConstexpr(),
            getMatchDeclName(v), v.Node->getType().getAsString()));
    //
    auto ir = Extractor.extract2(*Context,
        ifStmt(isExpansionInMainFile())
        .bind("i"));
    auto If = getASTNodes<clang::IfStmt>(ir, "i");
    for(auto v : If)
        IfStmts.emplace_back(CEIfStmt(v.Location, v.Node->isConstexpr()));
}

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CEIfStmt, Location, isConstexpr);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CEDecl, Location, isConstexpr, Identifier,
    Type);

template<typename T>
void ConstexprAnalysis::featuresToJSON(std::string Kind, const std::vector<T>& fs){
    for(auto f : fs){
        json f_j = f;
        Features[Kind].emplace_back(f_j);
    }
}

void ConstexprAnalysis::analyzeFeatures(){
    extractFeatures();
    // Fill JSON with data about function (templates)
    featuresToJSON("vars", Variables);
    featuresToJSON("non-member functions", NonMemberFunctions);
    featuresToJSON("member functions", MemberFunctions);
    featuresToJSON("if stmts", IfStmts);
}

unsigned countConstexprOccurences(const ordered_json& j){
    return std::count_if(j.begin(), j.end(),
        [](ordered_json k){
            return k.at("isConstexpr").get<bool>();
        });
}

void constexprPopToJSON(ordered_json& Statistics, const ordered_json& j, llvm::StringRef t){
    auto cco = countConstexprOccurences(j);
    Statistics["constexpr usage"][t.str()]["yes"] = cco;
    Statistics["constexpr usage"][t.str()]["no"] = j.size()-cco;
}

void ConstexprAnalysis::processFeatures(nlohmann::ordered_json j){
    for(auto t : {"vars", "non-member functions", "member functions", "if stmts"}){
        if(j.contains(t))
            constexprPopToJSON(Statistics, j.at(t), t);
    }
}

bool ConstexprAnalysis::s_registered =
    AnalysisFactory::RegisterAnalysis(ConstexprAnalysis::ShorthandName,
        ConstexprAnalysis::Create);

//-----------------------------------------------------------------------------
