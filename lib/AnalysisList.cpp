#include <iostream>

#include "llvm/ADT/StringRef.h"

#include "cxx-langstat/AnalysisList.h"

// Strongly inspired by clang-tidy's Globlist.h, GlobList.cpp:
// https://github.com/llvm/llvm-project/blob/release/11.x/clang-tools-extra/clang-tidy/GlobList.h
// https://github.com/llvm/llvm-project/blob/release/11.x/clang-tools-extra/clang-tidy/GlobList.cpp

//-----------------------------------------------------------------------------

std::string ConsumeAnalysis(llvm::StringRef& AnalysisList){
    llvm::StringRef UntrimmedAnalysis = AnalysisList.substr(0, AnalysisList.find(','));
    llvm::StringRef Analysis = UntrimmedAnalysis.trim(' ');
    AnalysisList = AnalysisList.substr(UntrimmedAnalysis.size() + 1);
    return Analysis.str();
}

AnalysisList::AnalysisList() : Items(){}

AnalysisList::AnalysisList(llvm::StringRef s){
    while(!s.empty()){
        AnalysisListItem Item;
        Item.Name = ConsumeAnalysis(s);
        Items.emplace_back(Item);
    }
}

bool AnalysisList::contains(std::string s){
    for(const AnalysisListItem& Item : Items){
        if(Item.Name==s)
            return true;
    }
    return false;
}

void AnalysisList::dump(){
    for(auto s : Items)
        std::cout << s.Name << ", ";
    std::cout << '\n';
}

//-----------------------------------------------------------------------------
