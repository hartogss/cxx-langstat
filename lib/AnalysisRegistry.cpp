#include <iostream>

#include "cxx-langstat/AnalysisRegistry.h"
#include "cxx-langstat/AnalysisList.h"

#include "cxx-langstat/Analyses/LoopDepthAnalysis.h"
#include "cxx-langstat/Analyses/LoopKindAnalysis.h"


AnalysisRegistry::AnalysisRegistry() {
    std::cout << "AR created" << std::endl;

    // auto t1 = std::make_unique<LoopDepthAnalysis>();
    // auto t2 = std::make_unique<LoopKindAnalysis>();
    //
    // // Analyses.emplace_back(std::move(t1));
    //
    // Analyses.emplace_back(std::move(t1));
    // Analyses.emplace_back(std::move(t2));
    createAllAnalyses();

}
AnalysisRegistry::AnalysisRegistry(std::string EAS) {
    std::cout << "AR2 created" << std::endl;
    createAllAnalyses();
    setEnabledAnalyses(EAS);

    auto t1 = std::make_unique<LoopDepthAnalysis>();
    Analyses.emplace_back(std::move(t1));
}
// AnalysisRegistry::~AnalysisRegistry(){
//     std::cout << "AR dtor" << std::endl;
// }
void AnalysisRegistry::createAllAnalyses(){
    std::cout << "Creating analyses" << std::endl;
    auto t1 = std::make_unique<LoopDepthAnalysis>();
    auto t2 = std::make_unique<LoopKindAnalysis>();
    Analyses.emplace_back(std::move(t1));
    Analyses.emplace_back(std::move(t2));
    Abbrev.emplace_back("lda");
    Abbrev.emplace_back("lka");
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
        AnalysisMapping[name]->run("testfile.cpp", Context);
        // AnalysisMapping[name]->print();
    }
}
