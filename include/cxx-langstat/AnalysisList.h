#ifndef ANALYSISLIST_H
#define ANALYSISLIST_H

#include <vector>
#include <string>
#include "llvm/ADT/StringRef.h"

// Strongly inspired by clang-tidy's Globlist.h:
// https://github.com/llvm/llvm-project/blob/release/11.x/clang-tools-extra/clang-tidy/GlobList.h

//-----------------------------------------------------------------------------

class AnalysisList {
private:
    struct AnalysisListItem {
        std::string Name; // This should be string, not StringRef
        bool operator<(const AnalysisListItem& other) const {
            return Name < other.Name;
        }
    };
public:
    AnalysisList();
    AnalysisList(llvm::StringRef s);
    std::vector<AnalysisListItem> Items;
    bool contains(std::string s);
    void dump();
};

//-----------------------------------------------------------------------------

#endif // ANALYSISLIST_H
