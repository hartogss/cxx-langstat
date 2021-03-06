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
    std::string getShorthand() override {
        return ShorthandName;
    }
private:
    void analyzeFeatures() override;
    void processFeatures(nlohmann::ordered_json j) override;
    //
    static constexpr auto ShorthandName = "lka";
};

//-----------------------------------------------------------------------------

#endif /* LOOPKINDANALYSIS_H */
