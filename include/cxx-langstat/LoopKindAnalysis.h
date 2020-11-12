#ifndef LOOPKINDANALISYS_H
#define LOOPKINDANALISYS_H

#include "cxx-langstat/Analysis.h"

class RangeBasedLoops : public Analysis {
public:
    RangeBasedLoops(clang::tooling::ClangTool Tool);
    void extract() override;
    void analyze() override;
    void run() override;
};

#endif /* LOOPKINDANALISYS_H */
