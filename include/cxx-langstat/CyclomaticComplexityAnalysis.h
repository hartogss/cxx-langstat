#ifndef CYCLOMATICCOMPLEXITYANALYSIS_H
#define CYCLOMATICCOMPLEXITYANALYSIS_H

#include "cxx-langstat/Analysis.h"

//-----------------------------------------------------------------------------

class CyclomaticComplexityAnalysis : public Analysis {
public:
    CyclomaticComplexityAnalysis(clang::ASTContext& Context);
    void extract() override;
    void analyze() override;
    void run() override;

};

#endif // CYCLOMATICCOMPLEXITY_H

//-----------------------------------------------------------------------------
