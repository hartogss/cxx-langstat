#ifndef ANALYSISLIST_H
#define ANALYSISLIST_H

#include <vector>

// 'Copy' of clang-tidy's Globlist.h

class AnalysisList {
private:
    struct AnalysisListItem {
        llvm::StringRef Name;
        bool operator<(const AnalysisListItem& other) const {
            return Name < other.Name;
        }
    };
public:
    AnalysisList();
    AnalysisList(llvm::StringRef s);
    std::vector<AnalysisListItem> Items;
    bool contains(std::string s);
};

#endif // ANALYSISLIST_H
