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
    void run(llvm::StringRef InFile, clang::ASTContext& Ctxt){
        Context = &Ctxt;
        analyzeFeatures();
    }
    // Return by ref is OK, as Analysis goes out of scope at the very end of the
    // program
    const nlohmann::ordered_json& getFeatures(){
        return Features;
    }
    const nlohmann::ordered_json& getStatistics(){
        return Statistics;
    }
    // Computes interesting statistics from features like prevalences or comparisons.
    virtual void processFeatures(nlohmann::ordered_json j) = 0;
protected:
    llvm::StringRef InFile;
    BaseExtractor Extractor;
    clang::ASTContext* Context;
    nlohmann::ordered_json Features;
    nlohmann::ordered_json Statistics;
    // Function that looks for features in code and creates a JSON object to
    // hold all interesting features.
    virtual void analyzeFeatures() = 0;
};

//-----------------------------------------------------------------------------

#endif // ANALYSIS_H
