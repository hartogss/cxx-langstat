#ifndef USINGANALYSIS_H
#define USINGANALYSIS_H

#include "cxx-langstat/Analysis.h"

//-----------------------------------------------------------------------------

// need analysis object since we want other analysis to inherit interface
class UsingAnalysis : public Analysis {
public:
    UsingAnalysis(){
        std::cout << "UA ctor\n";
    }
    ~UsingAnalysis(){
        std::cout << "UA dtor\n";
    }
private:
    Matches<clang::Decl> TypedefDecls;
    Matches<clang::Decl> TypeAliasDecls;
    Matches<clang::Decl> TypeAliasTemplateDecls;
    Matches<clang::Decl> TypedefTemplateDecls;
    Matches<clang::Decl> td;
    void extractFeatures();
    // Put data about some kind of raccourci (typedef/alias) into JSON
    template<typename T>
    void gatherData(std::string RaccourciKind, const Matches<T>& Matches);
    void analyzeFeatures() override;
    void processFeatures(nlohmann::ordered_json j) override;
};

//-----------------------------------------------------------------------------

#endif // USINGANALYSIS_H
