#ifndef TEMPLATEINSTANTIATIONANALYSIS_H
#define TEMPLATEINSTANTIATIONANALYSIS_H

#include "cxx-langstat/Analysis.h"

//-----------------------------------------------------------------------------

class TemplateInstantiationAnalysis : public Analysis {
public:
    // ctor that just looks for any instantiations, no restrictions
    TemplateInstantiationAnalysis();
    // ctor that is restricted to look for class insts with certain names,
    // and possibly doesn't look at func and var insts at all
    TemplateInstantiationAnalysis(bool analyzeClassInstsOnly,
        clang::ast_matchers::internal::Matcher<clang::NamedDecl> Names);
    ~TemplateInstantiationAnalysis(){
        std::cout << "TIA dtor\n";
    }
private:
    bool analyzeClassInstsOnly;
    clang::ast_matchers::DeclarationMatcher ClassInstMatcher;
    Matches<clang::ClassTemplateSpecializationDecl> ClassImplicitInsts;
    Matches<clang::ClassTemplateSpecializationDecl> ClassExplicitInsts;
    Matches<clang::DeclaratorDecl> ImplicitInsts;
    Matches<clang::FunctionDecl> FuncInsts;
    Matches<clang::VarTemplateSpecializationDecl> VarInsts;
    // Responsible to fill vectors of matches defined above
    void extractFeatures();
    void analyzeFeatures() override;
    void processJSON() override;
    // Get location of instantiation
    template<typename T>
    std::string getInstantiationLocation(const Match<T>& Match, bool isImplicit);
    // Get location of class instantiation, we need to be a bit more careful
    std::string getInstantiationLocation
        (const Match<clang::ClassTemplateSpecializationDecl>& Match,
            bool isImplicit);
    // Given matches representing the instantiations of some kind, gather
    // for each instantiation the instantiation arguments.
    template<typename T>
    void gatherInstantiationData(Matches<T>& Insts, std::string InstKind,
        bool AreImplicit);
};

//-----------------------------------------------------------------------------

#endif // TEMPLATEINSTANTIATIONANALYSIS_H
