#ifndef CONSTEXPRANALYSIS_H
#define CONSTEXPRANALYSIS_H

#include "cxx-langstat/Analysis.h"

//-----------------------------------------------------------------------------

struct CEInfo {
    CEInfo(unsigned Location, bool isConstexpr) : Location(Location),
        isConstexpr(isConstexpr) {}
    unsigned Location;
    bool isConstexpr;
};
// For variables, could possibly be ameliorated with information whether or
// not the type is instantiation dependent or not, which can be
// the case if the varDecl is part of a varTemplateDecl
struct CEDecl : public CEInfo {
    CEDecl(unsigned Location, bool isConstexpr,
        std::string Id, std::string Type) :
        CEInfo(Location, isConstexpr), Identifier(Id), Type(Type){}
    std::string Identifier;
    std::string Type;
};
struct CEIfStmt : public CEInfo {
    CEIfStmt(unsigned Location, bool isConstexpr) : CEInfo(Location, isConstexpr){}
};

class ConstexprAnalysis : public Analysis{
public:
    ConstexprAnalysis(){
        std::cout << "CEA ctor\n";
    }
    ~ConstexprAnalysis(){
        std::cout << "CEA dtor\n";
    }
    std::string getShorthand() override {
        return ShorthandName;
    }
private:
    std::vector<CEDecl> Variables;
    std::vector<CEDecl> NonMemberFunctions;
    std::vector<CEDecl> MemberFunctions;
    std::vector<CEIfStmt> IfStmts;
    void extractFeatures();
    void analyzeFeatures() override;
    void processFeatures(nlohmann::ordered_json j) override;
    template<typename T>
    void featuresToJSON(std::string Kind, const std::vector<T>& fs);
    static constexpr auto ShorthandName = "cea";
};

//-----------------------------------------------------------------------------

#endif // CONSTEXPRANALYSIS_H
