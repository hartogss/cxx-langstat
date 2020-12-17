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
    void extract();
    void analyze();
    void run(llvm::StringRef InFile, clang::ASTContext& Context) override;
};

#endif // CYCLOMATICCOMPLEXITY_H

//-----------------------------------------------------------------------------
