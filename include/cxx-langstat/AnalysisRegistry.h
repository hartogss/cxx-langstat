#ifndef ANALYSISREGISTRY_H
#define ANALYSISREGISTRY_H

#include <memory>
#include "cxx-langstat/Analysis.h"
#include "cxx-langstat/AnalysisList.h"
#include "cxx-langstat/Options.h"

//-----------------------------------------------------------------------------
// Options for a single invocation of CXXLangstatMain
struct CXXLangstatOptions {
    CXXLangstatOptions(Stage s, std::vector<std::string> OutputFiles,
    std::string AnalysesString) :
        OutputFiles(OutputFiles),
        Stage(s),
        EnabledAnalyses(AnalysisList(AnalysesString)) {
            auto& Items = EnabledAnalyses.Items;
            std::sort(Items.begin(), Items.end());
        }
    std::vector<std::string> OutputFiles;
    Stage Stage;
    AnalysisList EnabledAnalyses;
};

//-----------------------------------------------------------------------------
// Registry holding information about options used, output files and pointers
// to the actual analyses for a single CXXLangstatMain call.
class AnalysisRegistry {
public:
    AnalysisRegistry(CXXLangstatOptions Opts);
    ~AnalysisRegistry();
    std::vector<std::unique_ptr<Analysis>> Analyses;
    CXXLangstatOptions Options;
    // Since AnalysisRegistry holds for each input file analyzed by ClangTool
    // the corresponding output file, we can leverage that to ensure we
    // get the correct output file path.
    std::string getCurrentOutputFile(){
        return Options.OutputFiles.at(FileIndex++);
    }
    void printEnabledAnalyses(){
        Options.EnabledAnalyses.dump();
    }
    void createFreshAnalyses();
    void destroyAnalyses(){
        Analyses.clear();
    }
private:
    unsigned FileIndex = 0;
};

//-----------------------------------------------------------------------------

#endif // ANALYSISREGISTRY_H
