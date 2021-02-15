#ifndef LOOPKINDANALYSIS_H
#define LOOPKINDANALYSIS_H

#include "cxx-langstat/Analysis.h"

//-----------------------------------------------------------------------------

class LoopKindAnalysis : public Analysis {
public:
    LoopKindAnalysis(){
        std::cout << "LKA ctor\n";
    }
    ~LoopKindAnalysis(){
        std::cout << "LKA dtor\n";
    }
private:
    void analyzeFeatures() override;
    void processFeatures(nlohmann::ordered_json j) override;
    static std::unique_ptr<Analysis> Create(){
        return std::make_unique<LoopKindAnalysis>();
    }
    static bool s_registered;
};

//-----------------------------------------------------------------------------

#endif /* LOOPKINDANALYSIS_H */
