#include <iostream>
#include <vector>

#include "llvm/Support/raw_ostream.h"
#include "clang/AST/TemplateBase.h"

#include "cxx-langstat/Analyses/TemplateInstantiationAnalysis.h"
#include "cxx-langstat/Utils.h"

using namespace clang;
using namespace clang::ast_matchers;


//-----------------------------------------------------------------------------
// Compute statistics on arguments used to instantiate templates,
// no matter whether instantiation are explicit or implicit.
// Should be divided into 3 categories:
// - classes,
// - functions (including member methods), &
// - variables (including class static member variables, (not
// class field, those cannot be templated if they're not static))
// Template instantiations counted should stem from either explicit instantiations
// written by programmers or from implicit ones through 'natural usage'.
//
// Remember that template parameters can be non-types, types or templates.
// Goal: for each class, function, variable, report:
// - Number of instantiations
// - for individually non-types, types, templates parameters report how often
// they occured.
// - what those arguments actually were:
//      - non-types: what types did those parameters have, possible range of values used?
//      - types: what types were templates instantiated with?
//      - templates: names of template used.

void update(const TemplateArgument& TArg, std::map<std::string, unsigned>& Mapping){
    switch (TArg.getKind()){
        case TemplateArgument::Null:
        case TemplateArgument::NullPtr:
            break;
        case TemplateArgument::Type:
            Mapping["type"]++;
            break;
        case TemplateArgument::Declaration:
        case TemplateArgument::Integral:
            // FTInstNonTypeArguments++; break;
        case TemplateArgument::Template:
        Mapping["template"]++;
        break;
        // in case that it is a pack, can that be a pack of templates?
        // parameter pack can be of anything
    }
}

TemplateInstantiationAnalysis::TemplateInstantiationAnalysis
(clang::ASTContext& Context) : Analysis(Context) {
}
void TemplateInstantiationAnalysis::extract() {
    // auto m1 = classTemplateSpecializationDecl(
    //     isExpansionInMainFile(),
    //     has(
    //         cxxRecordDecl(isInstantiated())
    // )).bind("classinsts");

    // Oddly enough, the result of this matcher will give back a pointer to the
    // classTemplateSpecializationDecl containing the instantiated CXXRecordDecl,
    // not a pointer to the cxxRecordDecl. This is why the cast below is OK.
    // Note that because it returns ClassTemplateSpecializationDecl*, the
    // location reported here is wrong for implicit instantiations because they
    // are subtrees of the CTSD. Explicit instantiation locations are reported
    // correctly.
    auto ClassInstMatcher = cxxRecordDecl(
        isExpansionInMainFile(),
        isTemplateInstantiation())
    .bind("classinsts");
    auto ClassInsts = Extr.extract("classinsts", ClassInstMatcher);
    for(auto m : ClassInsts){
        std::cout << getDeclName(m) << " @ " << m.location << std::endl;
        if(auto Node = cast<ClassTemplateSpecializationDecl>(m.node)){
            // https://stackoverflow.com/questions/44397953/retrieve-template-parameters-from-cxxconstructexpr-in-clang-ast
            // Check why it works like this and the other does not
            TemplateArgumentList const& TAList(
                Node->getTemplateInstantiationArgs());
            // auto TAList = Node->getTemplateInstantiationArgs();
            for(unsigned idx=0; idx<TAList.size(); idx++){
                std::string res;
                auto TArg = TAList.get(idx);
                llvm::raw_string_ostream OS(res);
                TArg.dump(OS);
                std::cout << res << "\n";
            }
        }
    }
    std::cout << "--------\n";


    std::map<std::string, unsigned> FuncTempArgKinds {
        {"nontype", 0}, {"type", 0}, {"template", 0},
    };

    // In contrast, this result gives a pointer to a functionDecl, which has
    // too has a function we can call to get the template arguments.
    // Here, the location reported is in both explicit and implicit cases wrong,
    // the location where the function template is defined is returned
    // (to be precise, the line where the return value is specified).
    auto FuncInstMatcher = functionDecl(
        isExpansionInMainFile(),
        isTemplateInstantiation())
    .bind("funcinsts");
    auto FuncInsts = Extr.extract("funcinsts", FuncInstMatcher);
    for(auto m : FuncInsts){
        std::cout << getDeclName(m) << " @ " << m.location << std::endl;
        // auto specKind = cast<FunctionDecl>(m.node)->getTemplateSpecializationKindForInstantiation();
        // std::cout << "specialization kind: " << specKind << std::endl;
        if(auto Node = cast<FunctionDecl>(m.node)){
            auto TALPtr = Node->getTemplateSpecializationArgs();
            for(unsigned idx=0; idx<TALPtr->size(); idx++){
                std::string res;
                auto TArg = TALPtr->get(idx);
                llvm::raw_string_ostream OS(res);
                TArg.dump(OS);
                std::cout << res << "\n";
                update(TArg, FuncTempArgKinds);
                // std::cout << dyn_cast<FunctionDecl>(m.node)->getType().getTypePtr()->getTypeClassName() << std::endl;
            }
        }
    }
    for(auto [key, val] : FuncTempArgKinds){
        std::cout << key << ": " << val << "\n";
    }
    std::cout << "--------\n";




    // Same behavior as with classTemplates: gives pointer to a
    // varSpecializationDecl. However, the location reported is that of the
    // varDecl itself... no matter if explicit or implicit instantiation.
    auto VarInstMatcher = varDecl(
        isExpansionInMainFile(),
        isTemplateInstantiation())
    .bind("varinsts");
    auto VarInsts = Extr.extract("varinsts", VarInstMatcher);
    for(auto m : VarInsts){
        std::cout << getDeclName(m) << " @ " << m.location << std::endl;
        if(auto Node = cast<VarTemplateSpecializationDecl>(m.node)){
            TemplateArgumentList const& TAList(
                Node->getTemplateInstantiationArgs());
            for(unsigned idx=0; idx<TAList.size(); idx++){
                std::string res;
                auto TArg = TAList.get(idx);
                llvm::raw_string_ostream OS(res);
                TArg.dump(OS);
                std::cout << res << "\n";
            }
        }
        // old way by getting vardecl type
        // if(auto Node = cast<VarDecl>(m.node)){
        //     auto specKind = Node->getTemplateSpecializationKindForInstantiation();
        //     std::cout << "specialization kind: " << specKind << std::endl;
        //     std::cout << Node->getType().getTypePtr()->getTypeClassName() << std::endl;
        //     std::cout << "test: " << get(Node->getType().getTypePtr()) << std::endl;
        // }
    }
}

std::string get(const Type* t){
    if(auto tprime = dyn_cast<ElaboratedType>(t)){
        auto subt = tprime->getNamedType().getTypePtr();
        return get(subt);
    }
    if(auto tprime = dyn_cast<TemplateSpecializationType>(t)){
        auto numTargs = tprime->getNumArgs();
        std::string res = "";
        if(numTargs>1)
            res.append("(");
        for(unsigned idx=0; idx<numTargs; idx++){
            if(idx)
                res.append(",");
            auto Targ = tprime->getArg(idx);
            llvm::raw_string_ostream OS(res);
            Targ.dump(OS);
        }
        if(numTargs>1)
            res.append(")");
        return res;
    }
    if(auto tprime = dyn_cast<SubstTemplateTypeParmType>(t)){
        return tprime->getReplacementType().getAsString();
        // auto TTPT = n->getReplacedParameter();
        // std::cout << "pp " << TTPT->isParameterPack() << std::endl;
    }
    if(auto tprime = dyn_cast<BuiltinType>(t)){
        return tprime->desugar().getAsString();
    }
    return "fail";
}

void TemplateInstantiationAnalysis::analyze(){
}
void TemplateInstantiationAnalysis::run(){
    std::cout << "\033[32mRunning template instantiation analysis:\033[0m\n";
    extract();
    analyze();
}

//-----------------------------------------------------------------------------
