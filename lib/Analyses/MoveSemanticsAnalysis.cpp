#include "cxx-langstat/Analyses/MoveSemanticsAnalysis.h"

using namespace clang::ast_matchers;
using ojson = nlohmann::ordered_json;

// Functions to convert structs to/from JSON.
void to_json(nlohmann::json& j, const ConstructInfo& ci){
    j = nlohmann::json{
        {"function", ci.Func},
        {"function type", ci.FuncType},
        {"param", ci.ParamId},
        // {"argument", ci.ArgExpr},
        {"copy/move is compiler-generated", ci.CompilerGenerated},
        {"construction kind", toString.at(ci.CK)},
        {"call location"}, ci.CallLocation};
}
void from_json(const nlohmann::json& j, ConstructInfo& ci){
    ci.CK = fromString.at(j.at("CK").get<std::string>());

}

void MoveSemanticsAnalysis::CopyOrMoveAnalyzer::analyzeFeatures() {
    // auto m = callExpr(isExpansionInMainFile(),
        // hasAnyArgument(expr(hasType(recordType())).bind("expr")));

    // auto m = callExpr(isExpansionInMainFile(),
    //     forEachArgumentWithParam(
    //         expr(hasType(type())).bind("expr"),
    //         parmVarDecl(hasType(recordType()), isExpansionInMainFile()).bind("parm")));

    auto m = callExpr(isExpansionInMainFile(),
        forEachArgumentWithParam(
            cxxConstructExpr().bind("arg"),
            parmVarDecl(hasType(recordType()), isExpansionInMainFile()).bind("parm")))
            .bind("callexpr");


    auto Res = Extractor.extract2(*Context, m);
    auto Args = getASTNodes<clang::CXXConstructExpr>(Res, "arg");
    auto Parms = getASTNodes<clang::ParmVarDecl>(Res, "parm");
    auto Calls = getASTNodes<clang::CallExpr>(Res, "callexpr");

    int n = Args.size();
    for(int idx=0; idx<n; idx++) {
        auto p = Parms.at(idx);
        auto a = Args.at(idx);
        auto c = Calls.at(idx);
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
        std::cout  << a.Node->isElidable() << "\n";

        // what callee should we do here?
        CI.Func = c.Node->getDirectCallee()->getQualifiedNameAsString();
        CI.FuncType = c.Node->getDirectCallee()->getType().getAsString();
        CI.ParamId = p.Node->getQualifiedNameAsString();
        // CI.ArgLocation = a.Location;
        if(Ctor->isCopyOrMoveConstructor())
            CI.CK = static_cast<ConstructKind>(Ctor->isMoveConstructor()); // FIXME
        else
            CI.CK = ConstructKind::Unknown;
        CI.CompilerGenerated = Ctor->isImplicit();
        CI.CallLocation = c.Location;

        std::string Result;
        llvm::raw_string_ostream stream(Result);
        // a.Node->dump(stream, *Context); // why with context
        CI.ArgExpr = Result;


        nlohmann::json c_j = CI;
        Features.emplace_back(c_j);
    }
}
void MoveSemanticsAnalysis::CopyOrMoveAnalyzer::processFeatures(ojson j) {

}
