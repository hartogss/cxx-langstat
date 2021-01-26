#include <iostream>

#include "llvm/ADT/StringRef.h"

#include "cxx-langstat/AnalysisList.h"

// 'Copy' of clang-tidy's Globlist.cpp


std::string ConsumeAnalysis(llvm::StringRef& AnalysisList){
    llvm::StringRef UntrimmedAnalysis = AnalysisList.substr(0, AnalysisList.find(','));
    llvm::StringRef Analysis = UntrimmedAnalysis.trim(' ');
    AnalysisList = AnalysisList.substr(UntrimmedAnalysis.size() + 1);
    return Analysis.str();
}

AnalysisList::AnalysisList() : Items(){

}

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
