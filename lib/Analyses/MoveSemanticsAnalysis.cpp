#include "cxx-langstat/Analyses/MoveSemanticsAnalysis.h"

using namespace clang;
using namespace clang::ast_matchers;
using ojson = nlohmann::ordered_json;

//-----------------------------------------------------------------------------

namespace msa {

// Functions to convert structs to/from JSON.
void to_json(nlohmann::json& j, const FunctionParamInfo& o){
    j = nlohmann::json{
        {"Func Identifier", o.FuncId},
        {"Func Type", o.FuncType},
        {"Func Location", o.FuncLocation},
        {"Identifier", o.Id},
        {"construction kind", toString.at(o.CK)},
        {"copy/move ctor is compiler-generated", o.CompilerGenerated},
        {"Parm Type", o.ParmType}
        };
}
void from_json(const nlohmann::json& j, FunctionParamInfo& o){
    j.at("Func Identifier").get_to(o.FuncId);
    j.at("Func Type").get_to(o.FuncType);
    j.at("Func Location").get_to(o.FuncLocation);
    j.at("Identifier").get_to(o.Id);
    o.CK = fromString.at(j.at("construction kind").get<std::string>());
    j.at("copy/move ctor is compiler-generated").get_to(o.CompilerGenerated);
    j.at("Parm Type").get_to(o.ParmType);
}
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CallExprInfo, Location);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ConstructInfo, CallExpr, Parameter);


// ** StdMoveStdForwardUsageAnalyzer **
MoveSemanticsAnalysis::StdMoveStdForwardUsageAnalyzer::
StdMoveStdForwardUsageAnalyzer() : TemplateInstantiationAnalysis(
    InstKind::Function,
    clang::ast_matchers::hasAnyName("std::move", "std::forward"),
    // libc++
    // https://github.com/llvm/llvm-project/blob/release/11.x/libcxx/include/type_traits#L2613
    "type_traits|"
    // libstdc++
    // https://gcc.gnu.org/onlinedocs/libstdc++/libstdc++-api-4.5/a00936.html
    "bits/move.h"
    ){
        std::cout << "StdMoveStdForwardUsageAnalyzer\n";
}
void MoveSemanticsAnalysis::StdMoveStdForwardUsageAnalyzer::
processFeatures(nlohmann::ordered_json j) {
    if(j.contains(p1key) && j.at(p1key).contains(FuncKey)){
        templatePrevalence(j.at(p1key).at(FuncKey), Statistics);
    }
}


// ** CopyOrMoveAnalyzer **
void MoveSemanticsAnalysis::CopyOrMoveAnalyzer::analyzeFeatures() {
    // Gives us a triad of callexpr, a pass-by-value record type function
    // parameter and the expr that is the argument to that parameter.
    auto m = callExpr(isExpansionInMainFile(),
        forEachArgumentWithParam(
            // Argument is something that has to be constructed
            expr(anyOf(
                // for trivially destructible classes
                cxxConstructExpr().bind("arg"),
                // For clases that are not trivially destructible, i.e. have
                // custom destructor
                // https://clang.llvm.org/doxygen/classclang_1_1CXXBindTemporaryExpr.html#details
                // When the to be constructed type has a non-trivial destructor,
                // the clang AST wraps a CXXBindTemporaryExpr around the
                // CXXConstructExpr, and I don't understand why
                cxxBindTemporaryExpr(has(cxxConstructExpr().bind("arg"))))),
            // Type of parameter should by-value
            // and is not required to be isExpansionInMainFile, perfectly
            // fine to call function from other TU by value
            parmVarDecl(hasType(type(unless(referenceType())))).bind("parm")))
    .bind("callexpr");
    auto Res = Extractor.extract2(*Context, m);
    auto Args = getASTNodes<clang::CXXConstructExpr>(Res, "arg");
    auto Parms = getASTNodes<clang::ParmVarDecl>(Res, "parm");
    auto Calls = getASTNodes<clang::CallExpr>(Res, "callexpr");
    assert(Args.size() == Calls.size() && Parms.size() == Calls.size());

    int n = Args.size();
    for(int idx=0; idx<n; idx++) {
        auto p = Parms.at(idx);
        auto a = Args.at(idx);
        auto c = Calls.at(idx);
        // what callee should we get here?
        auto f = c.Node->getDirectCallee();
        FunctionParamInfo FPI;
        CallExprInfo CEI;
        ConstructInfo CI;
        auto Ctor = a.Node->getConstructor();

        LangOptions LO;
        PrintingPolicy PP(LO);
        PP.PrintCanonicalTypes = true;
        PP.SuppressTagKeyword = false;
        PP.SuppressScope = false;
        PP.SuppressUnwrittenScope = true;
        PP.FullyQualifiedName = true;
        PP.Bool = true;

        // std::cout << a.Location << ", " <<
        //     Ctor->getQualifiedNameAsString();
        // if(Ctor->isCopyConstructor()){
        //     std::cout << " copy, ";
        // } else if(Ctor->isMoveConstructor()){
        //     std::cout << " move, ";
        // } else if(Ctor->isDefaultConstructor()){
        //     std::cout << " def, ";
        // }
        // std::cout << a.Node->isElidable() << "\n";
        // if(auto r = clang::dyn_cast<clang::CXXTemporaryObjectExpr>(a.Node))
        //     std::cout << r->isElidable() << "\n";

        std::string Result;
        llvm::raw_string_ostream stream(Result);
        // Can't use getQualifiedNameAsString, will print unwritten scope
        // Can't use getNameForDiagnostic, will print specialization argument
        f->printQualifiedName(stream, PP);
        FPI.FuncId = Result;
        FPI.FuncType = f->getType().getAsString(PP);
        FPI.Id = p.Node->getQualifiedNameAsString();
        FPI.FuncLocation = Context->getFullLoc(f->getInnerLocStart())
            .getLineNumber();
        if(Ctor->isCopyOrMoveConstructor())
            FPI.CK = static_cast<ConstructKind>(Ctor->isMoveConstructor());
        else
            FPI.CK = ConstructKind::Unknown;
        FPI.CompilerGenerated = Ctor->isImplicit();
        FPI.ParmType = p.Node->getType().getAsString(PP);
        CEI.Location = c.Location;
        CI.Parameter = FPI;
        CI.CallExpr = CEI;
        nlohmann::json c_j = CI;
        Features.emplace_back(c_j);
    }
}

std::map<std::string, std::pair<unsigned, unsigned>>
NumCopiesAndMovesPerType(const ojson& j){
    std::map<std::string, std::pair<unsigned, unsigned>> m;
    for(const auto& ci_j : j){
        // std::cout << ci_j.dump(4) << std::endl;
        ConstructInfo ci;
        from_json(ci_j, ci);
        auto s = ci.Parameter.CK;
        auto Type = ci.Parameter.ParmType;
        m.try_emplace(Type, std::make_pair(0, 0));
        if(s == ConstructKind::Copy)
            m.at(Type).first++;
        if(s == ConstructKind::Move)
            m.at(Type).second++;
    }
    return m;
}

std::pair<unsigned, unsigned> NumCopiesAndMoves(const ojson& j){
    auto Copies = 0;
    auto Moves = 0;
    for(const auto& ci_j : j){
        // std::cout << ci_j.dump(4) << std::endl;
        ConstructInfo ci;
        from_json(ci_j, ci);
        auto s = ci.Parameter.CK;
        if(s == ConstructKind::Copy)
            Copies++;
        if(s == ConstructKind::Move)
            Moves++;
    }
    return std::make_pair(Copies, Moves);
}

void CopyAndMoveCounts(const ojson& j, ojson& res){
    auto [TotalCopies, TotalMoves] = NumCopiesAndMoves(j);
    res["total"]["copy"] = TotalCopies;
    res["total"]["move"] = TotalMoves;
    auto PerTypeCopiesMoves = NumCopiesAndMovesPerType(j);
    for(const auto& [key, val] : PerTypeCopiesMoves){
        res["per type"][key]["copy"] = val.first;
        res["per type"][key]["move"] = val.second;
    }
}

void MoveSemanticsAnalysis::CopyOrMoveAnalyzer::processFeatures(ojson j){
    // std::cout << j.dump(4) << std::endl;
    if(j.contains(p2key)){
        CopyAndMoveCounts(j.at(p2key), Statistics);
    }
    // std::cout << Statistics.dump(4) << std::endl;
}

} // namespace msa

//-----------------------------------------------------------------------------
