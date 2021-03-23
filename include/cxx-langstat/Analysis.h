#ifndef ANALYSIS_H
#define ANALYSIS_H

#include "clang/ASTMatchers/ASTMatchers.h"

#include <nlohmann/json.hpp>

#include "cxx-langstat/BaseExtractor.h"

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
    // Get either features or statistics, part of the interface.
    // Return by ref is OK, analysis goes out of scope just after this when particular
    // file is analyzed.
    const nlohmann::ordered_json& getFeatures(llvm::StringRef InFile, clang::ASTContext& Ctxt){
        Context = &Ctxt;
        analyzeFeatures();
        return Features;
    }
    const nlohmann::ordered_json& getStatistics(nlohmann::ordered_json j){
        processFeatures(j);
        return Statistics;
    }
    // Implement this as by returning private static constexpr member of
    // analysis.
    // Not very nice solution, but ensure that all analyses have name they return
    // and shorthand name can be used to register analysis with factory.
    virtual std::string getShorthand() = 0;
protected:
    llvm::StringRef InFile;
    BaseExtractor Extractor;
    clang::ASTContext* Context;
    nlohmann::ordered_json Features;
    nlohmann::ordered_json Statistics;
    
    // Internal functions that actual analyses have to implement to compute
    // features or statistics.

    // Function that looks for features in code and creates a JSON object to
    // hold all interesting features.
    virtual void analyzeFeatures() = 0;
    // Computes interesting statistics from features like prevalences or comparisons.
    virtual void processFeatures(nlohmann::ordered_json j) = 0;
};

//-----------------------------------------------------------------------------

#endif // ANALYSIS_H
