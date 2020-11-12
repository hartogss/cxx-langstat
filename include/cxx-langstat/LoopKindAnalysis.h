#ifndef LOOPKINDANALYSIS_H
#define LOOPKINDANALYSIS_H

#include "cxx-langstat/Analysis.h"

class LoopKindAnalysis : public Analysis {
public:
    LoopKindAnalysis(clang::tooling::ClangTool Tool);
    void extract() override;
    void analyze() override;
    void run() override;
};

#endif /* LOOPKINDANALYSIS_H */
