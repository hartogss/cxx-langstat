#ifndef TEMPLATE_H
#define TEMPLATE_H

#include "cxx-langstat/Analysis.h"

//-----------------------------------------------------------------------------

class Template : public Analysis {
public:
    Template(clang::ASTContext& Context);
    // step 1: extraction
    void extract() override;
    //step 2: compute stats
    void analyze() override;
    //step 3: visualization (for later)

    // combine
    void run() override;
};

//-----------------------------------------------------------------------------

#endif // TEMPLATE_H
