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
    void run(llvm::StringRef InFile, clang::ASTContext& Context){
        setContext(Context);
        analyzeFeatures();
        // processJSON
    }
    //
    const nlohmann::ordered_json& getResult(){
        return Result;
    }
protected:
    llvm::StringRef InFile;
    BaseExtractor Extractor;
    clang::ASTContext* Context;
    nlohmann::ordered_json Result;
    void setContext(clang::ASTContext& Ctxt){
        Context = &Ctxt;
    }
    // Function that looks for features in code and creates a JSON object to
    // hold all interesting features.
    virtual void analyzeFeatures() = 0;
    // Computes interesting statistics from features like prevalences or comparisons.
    virtual void processJSON() = 0;
};

//-----------------------------------------------------------------------------

#endif // ANALYSIS_H
