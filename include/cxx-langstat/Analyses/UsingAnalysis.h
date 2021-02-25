#ifndef USINGANALYSIS_H
#define USINGANALYSIS_H

#include "cxx-langstat/Analysis.h"

//-----------------------------------------------------------------------------

enum SynonymKind {
    Typedef, Alias
};
// Stores information about a single Synonym, i.e. a single typedef or type alias.
struct Synonym {
    Synonym(){}
    Synonym(int Location, SynonymKind Kind, bool Templated) :
        Location(Location),
        Kind(Kind),
        Templated(Templated){}
    int Location;
    SynonymKind Kind;
    bool Templated;
};

//-----------------------------------------------------------------------------

class UsingAnalysis : public Analysis {
public:
    UsingAnalysis(){
        std::cout << "UA ctor\n";
    }
    ~UsingAnalysis(){
        std::cout << "UA dtor\n";
    }
    std::string getShorthand() override {
        return ShorthandName;
    }
private:
    std::vector<Synonym> Synonyms;
    // Get matches from AST and store relevant information in "Synonyms"
    void extractFeatures();
    void analyzeFeatures() override;
    void processFeatures(nlohmann::ordered_json j) override;
    //
    static constexpr auto ShorthandName = "ua";
};

//-----------------------------------------------------------------------------

#endif // USINGANALYSIS_H
