#include "cxx-langstat/Analyses/MoveSemanticsAnalysis.h"

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
        {"copy/move ctor is compiler-generated", o.CompilerGenerated}
        };
}
void from_json(const nlohmann::json& j, FunctionParamInfo& o){
    j.at("Func Identifier").get_to(o.FuncId);
    j.at("Func Type").get_to(o.FuncType);
    j.at("Func Location").get_to(o.FuncLocation);
    j.at("Identifier").get_to(o.Id);
    o.CK = fromString.at(j.at("construction kind").get<std::string>());
    j.at("copy/move ctor is compiler-generated").get_to(o.CompilerGenerated);
}
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CallExprInfo, Location);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ConstructInfo, CallExpr, Parameter);


void MoveSemanticsAnalysis::CopyOrMoveAnalyzer::analyzeFeatures() {
    // Gives us a triad of callexpr, a pass-by-value record type function
    // parameter and the expr that is the argument to that parameter.
    auto m = callExpr(isExpansionInMainFile(),
        forEachArgumentWithParam(
            cxxConstructExpr().bind("arg"),
            // We want the type of the parameter variable to look like either
            // of the three below:
            // 1) func(C c){} for some class type C.
            // 2) func(C<T> c){} for some instantiation of class template T.
            // 3) template<typename T>
            //   func(T t){} for some type T that will be substituted.
            parmVarDecl(hasType(type(anyOf(
                recordType(), templateSpecializationType(), substTemplateTypeParmType())
                )), isExpansionInMainFile()).bind("parm")))
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
        auto f = c.Node->getDirectCallee();
        FunctionParamInfo FPI;
        CallExprInfo CEI;
        ConstructInfo CI;
        auto Ctor = a.Node->getConstructor();
        std::cout << a.Location << ", " <<
            Ctor->getQualifiedNameAsString();
        if(Ctor->isCopyConstructor()){
            std::cout << " copy, ";
        } else if(Ctor->isMoveConstructor()){
            std::cout << " move, ";
        } else if(Ctor->isDefaultConstructor()){
            std::cout << " def, ";
        }
        std::cout << a.Node->isElidable() << "\n";
        if(auto r = clang::dyn_cast<clang::CXXTemporaryObjectExpr>(a.Node))
            std::cout << r->isElidable() << "\n";

        // what callee should we do here?
        FPI.FuncId = f->getQualifiedNameAsString();
        FPI.FuncType = f->getType().getAsString();
        FPI.Id = p.Node->getQualifiedNameAsString();
        FPI.FuncLocation = Context->getFullLoc(f->getInnerLocStart())
            .getLineNumber();
        if(Ctor->isCopyOrMoveConstructor())
            FPI.CK = static_cast<ConstructKind>(Ctor->isMoveConstructor());
        else
            FPI.CK = ConstructKind::Unknown;
        FPI.CompilerGenerated = Ctor->isImplicit();
        CEI.Location = c.Location;
        CI.Parameter = FPI;
        CI.CallExpr = CEI;
        nlohmann::json c_j = CI;
        Features.emplace_back(c_j);
    }
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
    auto [Copy, Move] = NumCopiesAndMoves(j);
    res["copy"] = Copy;
    res["move"] = Move;
}


void MoveSemanticsAnalysis::CopyOrMoveAnalyzer::processFeatures(ojson j){
    // std::cout << j.dump(4) << std::endl;
    if(j.contains(p2desc)){
        CopyAndMoveCounts(j.at(p2desc), Statistics);
    }
    std::cout << Statistics.dump(4) << std::endl;

}

} // namespace msa

//-----------------------------------------------------------------------------
