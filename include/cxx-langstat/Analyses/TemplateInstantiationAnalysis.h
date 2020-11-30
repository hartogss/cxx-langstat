#ifndef TEMPLATEINSTANTIATIONANALYSIS_H
#define TEMPLATEINSTANTIATIONANALYSIS_H

#include "cxx-langstat/Analysis.h"

//-----------------------------------------------------------------------------

class TemplateInstantiationAnalysis : public Analysis {
public:
    TemplateInstantiationAnalysis(clang::ASTContext& Context);
    void extract() override;
    void analyze() override;
    void run() override;
};

//-----------------------------------------------------------------------------

#endif // TEMPLATEINSTANTIATIONANALYSIS_H
