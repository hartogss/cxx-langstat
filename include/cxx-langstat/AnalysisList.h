#ifndef ANALYSISLIST_H
#define ANALYSISLIST_H

#include <vector>

// 'Copy' of clang-tidy's Globlist.h

class AnalysisList {
public:
    AnalysisList();
    AnalysisList(llvm::StringRef s);
// private:
    struct AnalysisListItem {
        llvm::StringRef Name;
    };
    std::vector<AnalysisListItem> Items;
};

#endif // ANALYSISLIST_H
