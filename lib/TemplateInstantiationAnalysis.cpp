#include <iostream>
#include <vector>

#include "llvm/Support/raw_ostream.h"

#include "cxx-langstat/TemplateInstantiationAnalysis.h"

using namespace clang;
using namespace clang::ast_matchers;

std::string getDeclName(Match<clang::Decl> node){
    if(auto n = dyn_cast<clang::NamedDecl>(node.node)){
        return n->getNameAsString();
    } else {
        std::cout << "Decl @ " << node.location << "cannot be resolved" << std::endl;
        return "INVALID";
    }
}

//-----------------------------------------------------------------------------
// Compute statistics on arguments used to instantiate templates.
// Should be divided into 3 categories:
// - classes,
// - functions (including member methods), &
// - variables (including class static member variables, (not
// class field, those cannot be templated if they're not static))
// Template instantiations counted should stem from either explicit instantiations
// written by programmers or from implicit ones through 'natural usage'.
//
// Remember that template parameters can be non-types, types or templates.


TemplateInstantiationAnalysis::TemplateInstantiationAnalysis
(clang::ASTContext& Context) : Analysis(Context) {
}
void TemplateInstantiationAnalysis::extract() {
    // auto m1 = cxxRecordDecl(isTemplateInstantiation()).bind("classinsts");

    auto m2 = functionDecl(isTemplateInstantiation()).bind("funcinsts");
    auto FuncInsts = Extr.extract("funcinsts", m2);
    for(auto m : FuncInsts){
        std::cout << getDeclName(m) << std::endl;
        // auto specKind = cast<FunctionDecl>(m.node)->getTemplateSpecializationKindForInstantiation();
        // std::cout << specKind << std::endl;
        auto TALptr = cast<FunctionDecl>(m.node)->getTemplateSpecializationArgs();
        for(auto idx=0; idx<TALptr->size(); idx++){
            std::string res;
            auto Targ = TALptr->get(idx);
            llvm::raw_string_ostream OS(res);
            Targ.dump(OS);
            std::cout << res << "\n";
        }

    }
}
void TemplateInstantiationAnalysis::analyze(){
}
void TemplateInstantiationAnalysis::run(){
    extract();
    analyze();
}

//-----------------------------------------------------------------------------
