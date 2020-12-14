#ifndef LOOPKINDANALYSIS_H
#define LOOPKINDANALYSIS_H

#include "cxx-langstat/Analysis.h"

class LoopKindAnalysis : public Analysis {
public:
    LoopKindAnalysis();
    void extract(clang::ASTContext& Context);
    void run(llvm::StringRef InFile, clang::ASTContext& Context) override;
};

#endif /* LOOPKINDANALYSIS_H */
