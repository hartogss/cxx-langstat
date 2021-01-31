#ifndef ANALYSIS_H
#define ANALYSIS_H

#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

#include <nlohmann/json.hpp>

#include "BaseExtractor.h"

//-----------------------------------------------------------------------------
// Abstract Analysis class. Specific analyses should subclass this &
// implement its methods.

class Analysis {
public:
    // ctor
    Analysis(){}
    // dtor, virtual to ensure derived concrete analyses' dtors is called
    virtual ~Analysis() = default;

    // Run analysis
    // Return by ref is OK, as Analysis goes out of scope at the very end of the
    // program.
    const nlohmann::ordered_json& getFeatures(llvm::StringRef InFile, clang::ASTContext& Ctxt){
        ResetAnalysis();
        Context = &Ctxt;
        analyzeFeatures();
        return Features;
    }
    const nlohmann::ordered_json& getStatistics(nlohmann::ordered_json j){
        processFeatures(j);
        return Statistics;
    }
protected:
    llvm::StringRef InFile;
    BaseExtractor Extractor;
    clang::ASTContext* Context;
    nlohmann::ordered_json Features;
    nlohmann::ordered_json Statistics;
    // Function that looks for features in code and creates a JSON object to
    // hold all interesting features.
    virtual void analyzeFeatures() = 0;
    // Computes interesting statistics from features like prevalences or comparisons.
    virtual void processFeatures(nlohmann::ordered_json j) = 0;
    // As an analysis is reused for every file it is run on and analysis state
    // is kept until it is destroyed, we sometimes need to reset the state of
    // some member variables of the analysis. Often this is not necessary,
    // because the assignment operator "=" will overwrite old state, but when
    // it is not guaranteed that the old state will be overwritten (e.g. because)
    // the variable are modified using push_back, emplace_back, then we first
    // need to clean/reset those member variables.
    virtual void ResetAnalysis(){}
};

//-----------------------------------------------------------------------------

#endif // ANALYSIS_H
