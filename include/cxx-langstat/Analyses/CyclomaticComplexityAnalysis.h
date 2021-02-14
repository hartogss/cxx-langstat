#ifndef CYCLOMATICCOMPLEXITYANALYSIS_H
#define CYCLOMATICCOMPLEXITYANALYSIS_H

#include "cxx-langstat/Analysis.h"

//-----------------------------------------------------------------------------

class CyclomaticComplexityAnalysis : public Analysis {
public:
    CyclomaticComplexityAnalysis(){
        std::cout << "CCA ctor\n";
    }
    ~CyclomaticComplexityAnalysis(){
        std::cout << "CCA dtor\n";
    }
private:
    // Extracts features, calculates cyclomatic complexity and creates JSON
    // objects all in one go, since that is all pretty simple.
    void analyzeFeatures() override;
    void processFeatures(nlohmann::ordered_json j) override;
};

#endif // CYCLOMATICCOMPLEXITY_H

//-----------------------------------------------------------------------------
