#ifndef MOVESEMANTICSANALYSIS_H
#define MOVESEMANTICSANALYSIS_H

#include "cxx-langstat/Analysis.h"
#include "cxx-langstat/Utils.h"

#include "cxx-langstat/Analyses/TemplateInstantiationAnalysis.h"


//
enum class ConstructKind {
    Copy, Move, Unknown
};
const std::map<ConstructKind, std::string> toString = {{ConstructKind::Copy, "copy"},
    {ConstructKind::Move, "move"}, {ConstructKind::Unknown, "unknown"}};

const std::map<std::string, ConstructKind> fromString = {{"copy", ConstructKind::Copy},
    {"move", ConstructKind::Move}, {"unknown", ConstructKind::Unknown}};
//
struct ConstructInfo {
    std::string Func;
    std::string FuncType;
    unsigned FuncLocation;
    std::string ParamId;
    ConstructKind CK;
    bool CompilerGenerated;
    unsigned CallLocation;
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
        // Features = p1.getFeatures(InFile, *Context);
        Features = p2.getFeatures(InFile, *Context);
    }
    void processFeatures(nlohmann::ordered_json j) override {

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
            nlohmann::ordered_json res;
            typePrevalence(j, res);
            Statistics["algorithm type prevalence"] = res;
        }
    };
    // Examine when calling functions that pass by value, how often copy and
    // move constructors are used to construct the value of the callee.
    class CopyOrMoveAnalyzer : public Analysis {
    private:
        void analyzeFeatures() override;
        void processFeatures(nlohmann::ordered_json j) override;
    };
    MoveAndForwardUsageAnalyzer p1;
    CopyOrMoveAnalyzer p2;
};

#endif // MOVESEMANTICSANALYSIS_H
