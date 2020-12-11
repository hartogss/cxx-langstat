#include <iostream>

#include "cxx-langstat/AnalysisRegistry.h"
#include "cxx-langstat/AnalysisList.h"

#include "cxx-langstat/Analyses/LoopDepthAnalysis.h"
#include "cxx-langstat/Analyses/LoopKindAnalysis.h"


AnalysisRegistry::AnalysisRegistry(std::string EAS) {
    std::cout << "AR created" << std::endl;
    createAllAnalyses();
    setEnabledAnalyses(EAS);
}
// AnalysisRegistry::~AnalysisRegistry(){
//     std::cout << "AR dtor" << std::endl;
// }
void AnalysisRegistry::createAllAnalyses(){
    std::cout << "Creating analyses" << std::endl;
    // auto t1 = std::make_unique<LoopDepthAnalysis>();
    // auto t2 = std::make_unique<LoopKindAnalysis>();
    // AnalysisMapping.emplace("lda", std::move(t1));
    // AnalysisMapping.emplace("lka", std::move(t2));
    // std::cout << AnalysisMapping["lda"].get() << std::endl;
    AnalysisMapping.emplace("lda", new LoopDepthAnalysis());
}

void AnalysisRegistry::setEnabledAnalyses(std::string EAS){
    std::cout << "Enabling analyses: ";
    AnalysisList List(EAS);
    std::cout << "Number of analyses: " << List.Items.size() << std::endl;
    EnabledAnalyses = List; //why not use copy constructor,move ctor
}

void AnalysisRegistry::runEnabledAnalyses(llvm::StringRef InFile, clang::ASTContext& Context){
    std::cout << "Running analyses." << std::endl;
    for(auto EA : EnabledAnalyses.Items){
        auto name = EA.Name.str();
        // AnalysisMapping[name]->run("weirdfile.cpp", Context);
        AnalysisMapping[name]->print();
    }
}
