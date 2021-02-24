#ifndef ANALYSIS_H
#define ANALYSIS_H

#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

#include <nlohmann/json.hpp>

#include "BaseExtractor.h"
#include "cxx-langstat/Factory.h"

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
        Context = &Ctxt;
        analyzeFeatures();
        return Features;
    }
    const nlohmann::ordered_json& getStatistics(nlohmann::ordered_json j){
        processFeatures(j);
        return Statistics;
    }
    // implement this as by returning private static constexpr member of
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
    // Function that looks for features in code and creates a JSON object to
    // hold all interesting features.
    virtual void analyzeFeatures() = 0;
    // Computes interesting statistics from features like prevalences or comparisons.
    virtual void processFeatures(nlohmann::ordered_json j) = 0;
};

//-----------------------------------------------------------------------------

#endif // ANALYSIS_H
