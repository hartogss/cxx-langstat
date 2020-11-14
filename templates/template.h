#ifndef TEMPLATE_H
#define TEMPLATE_H

#include "cxx-langstat/Analysis.h"

//-----------------------------------------------------------------------------

class Template : public Analysis {
public:
    Template(clang::tooling::ClangTool Tool);
    // step 0: createMatcher(s)/getMatchers? idea is to have library of often-used
    // matchers or maybe even some datastructure of important matches itself
    // step 1: extraction
    void extract() override;
    //step 2: compute stats
    void analyze() override;
    //step 3: visualization (for later)
    // combine
    void run() override;
};

void runclangtidy();

//-----------------------------------------------------------------------------

#endif // TEMPLATE_H
