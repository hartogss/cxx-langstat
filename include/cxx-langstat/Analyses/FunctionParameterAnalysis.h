#ifndef FUNCTIONPARAMETERANALYSIS_H
#define FUNCTIONPARAMETERANALYSIS_H

#include "cxx-langstat/Analysis.h"

//-----------------------------------------------------------------------------
// https://stackoverflow.com/questions/7312745/static-cast-for-user-defined-types
// Basic information we want to capture both about functions and parameters
struct BasicInfo {
    BasicInfo() = default;
    BasicInfo(int Location, std::string Identifier) :
        Location(Location),
        Identifier(Identifier){}
    int Location;
    std::string Identifier;
};
// Holds for each kind of parameter how often it occured in function decl.
struct FunctionInfo : public BasicInfo {
    FunctionInfo() = default;
    int ByValue = 0;
    int ByNonConstLvalueRef = 0;
    int ByConstLvalueRef = 0;
    int ByRvalueRef = 0;
    // Non-canonical, sugared signature/type of the function.
    // Should not be used except for debugging purposes, e.g. check if
    // reported features make sense etc.
    std::string Signature;
};
struct FunctionTemplateInfo : public FunctionInfo {
    int ByUniversalRef = 0;
};
enum class ParmKind {
    Value, NonConstLValueRef, ConstLValueRef, RValueRef, UniversalRef
};
// Holds for what kind of passing is used for that parameter.
struct ParmInfo : public BasicInfo {
    ParmInfo() = default;
    ParmInfo(int Location, std::string Identifier, std::string Type) :
        Type(Type), BasicInfo(Location, Identifier){}
    ParmKind Kind;
    bool isInstantiationDependent;
    // Canonical, qualified, desugared type of the parameter. Can be used
    // to compare types of parameters.
    std::string Type;
};

//-----------------------------------------------------------------------------

class FunctionParameterAnalysis : public Analysis {
public:
    FunctionParameterAnalysis(){
        std::cout << "FPA ctor\n";
    }
    ~FunctionParameterAnalysis(){
        std::cout << "FPA dtor\n";
    }
    std::string getShorthand() override {
        return ShorthandName;
    }
private:
    std::vector<FunctionInfo> Functions;
    std::vector<FunctionTemplateInfo> FunctionTemplates;
    std::vector<ParmInfo> Parms;
    // Helper functions for analyzeFeatures
    void addFunction(const Match<clang::FunctionDecl>& match,
        std::map<std::string, bool> ParmMap);
    void addFunction(const Match<clang::FunctionTemplateDecl>& match,
        std::map<std::string, bool> ParmMap);
    template<typename T>
    // Helper function for extractFeatures: for each match figures if it has any
    // of the 4 kinds of parameters and puts it in one of the above vectors
    void associateParameters(const Matches<T>& Matches);
    void extractFeatures();
    void analyzeFeatures() override;
    void processFeatures(nlohmann::ordered_json j) override;
    // Helper function to gather data about functions or parameters into vector
    template<typename T>
    void gatherData(std::string DeclKind,
        const std::vector<T>& fs);
    static constexpr auto ShorthandName = "fpa";
};

#endif // FUNCTIONPARAMETERANALYSIS_H

//-----------------------------------------------------------------------------
