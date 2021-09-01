
#ifndef LLVMCONTAINERSANALYSIS_H
#define LLVMCONTAINERSANALYSIS_H

#include "cxx-langstat/Analyses/TemplateInstantiationAnalysis.h"

//-----------------------------------------------------------------------------

class LLVMContainersAnalysis : public TemplateInstantiationAnalysis {
public:
    LLVMContainersAnalysis();
    std::string getShorthand() override {
        return ShorthandName;
    }
private:
    // Don't need this function as we inherit from TIA
    // void analyzeFeatures() override;
    void processFeatures(nlohmann::ordered_json j) override;
    static constexpr auto ShorthandName = "lca";
    const std::string key = "llvm_container_prevalence";
};

//-----------------------------------------------------------------------------

#endif // LLVMCONTAINERSANALYSIS_H
