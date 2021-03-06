#ifndef MOVESEMANTICSANALYSIS_H
#define MOVESEMANTICSANALYSIS_H

#include "cxx-langstat/Analyses/TemplateInstantiationAnalysis.h"
#include "cxx-langstat/Utils.h"

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
    std::string ParmType;
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
// MSA is guaranteed to be called through analyzeFeatures and processFeatures.
class MoveSemanticsAnalysis : public Analysis {
public:
    MoveSemanticsAnalysis(){
        std::cout << "MSA ctor\n";
    }
    ~MoveSemanticsAnalysis(){
        std::cout << "MSA dtor\n";
    }

private:
    void analyzeFeatures() override {
        Features[p1key] = p1.getFeatures(InFile, *Context);
        Features[p2key] = p2.getFeatures(InFile, *Context);
    }
    void processFeatures(nlohmann::ordered_json j) override {
        Statistics[p1key] = p1.getStatistics(j);
        Statistics[p2key] = p2.getStatistics(j);
    }
    std::string getShorthand() override {
        return ShorthandName;
    }
    // Find how often std::move, std::forward from <utility> are used.
    struct StdMoveStdForwardUsageAnalyzer : public TemplateInstantiationAnalysis {
        StdMoveStdForwardUsageAnalyzer();
        void processFeatures(nlohmann::ordered_json j) override;
        std::string getShorthand() override { return "msap1"; }
    };
    // Examine when calling functions that pass by value, how often copy and
    // move constructors are used to construct the value of the callee.
    struct CopyOrMoveAnalyzer : public Analysis {
        CopyOrMoveAnalyzer() { std::cout << "CopyOrMoveAnalyzer\n"; }
        void analyzeFeatures() override;
        void processFeatures(nlohmann::ordered_json j) override;
        std::string getShorthand() override { return "msap2"; }
    };

    // Analyzers run by MSA
    StdMoveStdForwardUsageAnalyzer p1;
    CopyOrMoveAnalyzer p2;

    // JSON keys
    static constexpr auto p1key = "std::move, std::forward usage";
    static constexpr auto p2key = "copy or move construction";

    static constexpr auto ShorthandName = "msa";
};

} // namespace msa

#endif // MOVESEMANTICSANALYSIS_H
