// Thanks to Peter Goldsborough for sharing his implementation of CYC/McCabe:
// https://github.com/peter-can-talk/cppnow-2017/blob/master/code/mccabe/mccabe.cpp

#include "llvm/Support/raw_os_ostream.h"
#include "clang/Analysis/CFG.h"

#include <iostream>
#include <map>
#include <string>

#include "cxx-langstat/Analyses/CyclomaticComplexityAnalysis.h"

using namespace clang;
using namespace clang::ast_matchers;
using ordered_json = nlohmann::ordered_json;

//-----------------------------------------------------------------------------

void CyclomaticComplexityAnalysis::analyzeFeatures(){
    // Extract function features
    auto id = "fd";
    auto fDecl = functionDecl(
        isExpansionInMainFile(),
        // Should we remove this condition?
        unless(isImplicit()), // Should not be compiler-generated
        hasBody(anything()))  // Should be defined, i.e have a body
    .bind(id);
    auto matches = Extractor.extract(*Context, id, fDecl);

    // Directly create JSON objects that holds for each function the cyclomatic
    // complexity.
    ordered_json fdecls;
    for(auto match : matches){
        // http://clang.llvm.org/doxygen/classclang_1_1CFG.html#details
        // CFG is intraprocedural flow of a statement.
        // Statement can be a function body or a single expression.
        const std::unique_ptr<clang::CFG> cfg =
        clang::CFG::buildCFG(match.Node, match.Node->getBody(), // probably because body is compoundStmt
            Context, clang::CFG::BuildOptions()
        );
        // FIXME: remove this if we don't compute CYC of function templates in the future anyway.

        LangOptions LO;
        PrintingPolicy PP(LO);
        PP.PrintCanonicalTypes = true;
        PP.SuppressTagKeyword = false;
        PP.SuppressScope = false;
        PP.SuppressUnwrittenScope = true;
        PP.FullyQualifiedName = true;

        if(cfg){
            // Wanna print the cfgs? Don't, they're ugly.
            // llvm::raw_os_ostream OS(std::cout);
            // cfg->print(OS, clang::LangOptions(), true);
            // McCabe's formula for computing CYC should be applicable here
            // because intraprocedural CFGs in Clang have a distinct entry and
            // exit block.
            unsigned numNodes = cfg->size();
            unsigned numEdges = 0;
            for(auto block = cfg->begin(); block != cfg->end(); block++)
                numEdges += (*block)->succ_size();
            unsigned CYC = numEdges - numNodes + 2; // 2 since #connected components P=1
            // Use emplace back because of overloading
            fdecls[match.getDeclName(PP)].emplace_back(CYC);
        } else {
            std::cout << "\"" << match.getDeclName(PP) << "\" of type \""
            << cast<clang::FunctionDecl>(match.Node)->getType().getAsString()
            << "\" could not be analyzed. This is likely because this function "
                "is part of a function template.\n";
        }
    }
    Features["fdecls"] = fdecls;
}

// Assumes JSON object where analysis abbreviation (i.e. cca in this case)
// is already stripped off
void CyclomaticComplexityAnalysis::processFeatures(nlohmann::ordered_json j){
    std::map<std::string, unsigned> m;
    for(const auto& [fdeclname, cycs] : j["fdecls"].items()){
        for(auto cyc : cycs){
            // std::cout << cyc << std::endl;
            m.try_emplace(to_string(cyc), 0);
            m.at(to_string(cyc))++;
        }
    }
    // for(auto [key, val]:m)
        // std::cout << key << ", " << val << std::endl;
    std::string desc = "distribution of cyclomatic complexity of functions";
    Statistics[desc] = m;
}

//-----------------------------------------------------------------------------
