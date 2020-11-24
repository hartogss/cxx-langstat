#ifndef VARIABLETEMPLATEANALYSIS_H
#define VARIABLETEMPLATEANALYSIS_H

#include "cxx-langstat/Analysis.h"

//-----------------------------------------------------------------------------

class VariableTemplateAnalysis : public Analysis {
public:
    VariableTemplateAnalysis(clang::ASTContext& Context);
    void extract() override;
    void analyze() override;
    void run() override;
};

//-----------------------------------------------------------------------------

#endif // VARIABLETEMPLATEANALYSIS_H
