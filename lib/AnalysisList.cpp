#include <iostream>

#include "llvm/ADT/StringRef.h"

#include "cxx-langstat/AnalysisList.h"

// 'Copy' of clang-tidy's Globlist.cpp


llvm::StringRef ConsumeAnalysis(llvm::StringRef& AnalysisList){
    llvm::StringRef UntrimmedAnalysis = AnalysisList.substr(0, AnalysisList.find(','));
    llvm::StringRef Analysis = UntrimmedAnalysis.trim(' ');
    AnalysisList = AnalysisList.substr(UntrimmedAnalysis.size() + 1);
    return Analysis;
}

AnalysisList::AnalysisList() : Items(){

}

AnalysisList::AnalysisList(llvm::StringRef Ans){
    while(!Ans.empty()){
        AnalysisListItem Item;
        Item.Name = ConsumeAnalysis(Ans);
        std::cout << Item.Name.str();
        Items.emplace_back(Item);
    }
    std::cout << std::endl;
}

bool AnalysisList::contains(std::string s){
    for(const AnalysisListItem& Item : Items){
        if(Item.Name.str()==s)
            return true;
    }
    return false;
}
