#ifndef MOVESEMANTICSANALYSIS_H
#define MOVESEMANTICSANALYSIS_H

#include "cxx-langstat/Analysis.h"
#include "cxx-langstat/Utils.h"

#include "cxx-langstat/Analyses/TemplateInstantiationAnalysis.h"

namespace msa {

// Enum of the parmVarDecl of a functionDecl was constructed from the argument
// of a callExpr. Also, maps for prettier printing.
enum class ConstructKind { Copy, Move, Unknown };
const std::array<std::string, 3> S = {"copy", "move", "unknown"};
const std::map<ConstructKind, std::string> toString = {{ConstructKind::Copy, S[0]},
    {ConstructKind::Move, S[1]}, {ConstructKind::Unknown, S[2]}};
const std::map<std::string, ConstructKind> fromString = {{S[0], ConstructKind::Copy},
    {S[1], ConstructKind::Move}, {S[2], ConstructKind::Unknown}};
// Holds relevant information about a parmVarDecl.
struct FunctionParamInfo {
    std::string FuncId;
    std::string FuncType;
    unsigned FuncLocation;
    std::string Id;
    ConstructKind CK;
    bool CompilerGenerated;
};
struct CallExprInfo {
    unsigned Location;
};
// Holds info about parmVarDecl and the corresponding callExpr that causes it
// to be copied or moved to.
struct ConstructInfo {
    CallExprInfo CallExpr;
    FunctionParamInfo Parameter;
};

// Need to make MSA that adheres to Analysis interface, but that can call two
// independent other analyses.
// MSA is guaranteed to be called thru analyzeFeatures and processFeatures.
class MoveSemanticsAnalysis : public Analysis {
public:
    MoveSemanticsAnalysis() : p1(MoveAndForwardUsageAnalyzer()){
        std::cout << "MSA ctor\n";
    }
    ~MoveSemanticsAnalysis(){
        std::cout << "MSA dtor\n";
    }
private:
    void analyzeFeatures() override {
        Features[p1desc] = p1.getFeatures(InFile, *Context);
        Features[p2desc] = p2.getFeatures(InFile, *Context);
    }
    void processFeatures(nlohmann::ordered_json j) override {
        Statistics[p1desc] = p1.getStatistics(j);
        Statistics[p2desc] = p2.getStatistics(j);
    }

    // Find how often std::move, std::forward from <utility> are used.
    class MoveAndForwardUsageAnalyzer : public TemplateInstantiationAnalysis {
    public:
        MoveAndForwardUsageAnalyzer() :
        TemplateInstantiationAnalysis(InstKind::Function,
            clang::ast_matchers::hasAnyName("std::move", "std::forward"),
                "type_traits"){ // Why do these seem to be in the type_traits header?
                // https://stackoverflow.com/questions/59860733/why-is-stdmove-defined-in-type-traits-but-not-in-utility
                std::cout << "msap1\n";
        }
        void processFeatures(nlohmann::ordered_json j) override{
            if(j.contains(p1desc) && j.at(p1desc).contains("func insts")){
                typePrevalence(j.at(p1desc).at("func insts"), Statistics);
            }
        }
    };
    // Examine when calling functions that pass by value, how often copy and
    // move constructors are used to construct the value of the callee.
    class CopyOrMoveAnalyzer : public Analysis {
    private:
        void analyzeFeatures() override;
        void processFeatures(nlohmann::ordered_json j) override;
    };
    // Analyzers run by MSA
    MoveAndForwardUsageAnalyzer p1;
    CopyOrMoveAnalyzer p2;
    constexpr static auto p1desc = "std::move, std::forward usage";
    constexpr static auto p2desc = "copy or move construction";
    static std::unique_ptr<Analysis> Create(){
        return std::make_unique<MoveSemanticsAnalysis>();
    }
    static bool s_registered;
};

} // namespace msa

#endif // MOVESEMANTICSANALYSIS_H
