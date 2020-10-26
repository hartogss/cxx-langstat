#ifndef FORSTMTANALYSIS_H
#define FORSTMTANALYSIS_H

#include "Analysis.h"
#include "clang/Tooling/CommonOptionsParser.h"

namespace fsa {

class ForStmtAnalysis : public Analysis {
    using Analysis::Analysis;
};

ForStmtAnalysis newForStmtAnalysis(llvm::cl::opt<int> FSOption);

}

#endif /* FORSTMTANALYSIS_H */
