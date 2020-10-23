#ifndef FORSTMTANALYSIS_H
#define FORSTMTANALYSIS_H

#include "Analysis.h"
#include "clang/Tooling/CommonOptionsParser.h"


class ForStmtAnalysis : public Analysis {
    using Analysis::Analysis;
};

extern ForStmtAnalysis forstmt;

#endif /* FORSTMTANALYSIS_H */
