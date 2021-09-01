
#include "cxx-langstat/Analyses/LLVMContainersAnalysis.h"
#include "cxx-langstat/Utils.h"

LLVMContainersAnalysis::LLVMContainersAnalysis() : TemplateInstantiationAnalysis(
    InstKind::Class,
    clang::ast_matchers::hasAnyName(
    	// LLVM containers
    	// Sequential containers
    	"llvm::ArrayRef", "llvm::TinyPtrVector", "llvm::SmallVector",
        "llvm::SmallVectorImpl",
        "llvm::PackedVector",
    	// String-like containers
    	"llvm::StringRef", "llvm::Twine", "llvm::SmallString",
    	// Set-like containers
    	"llvm::SmallSet", "llvm::SmallPtrSet", "llvm::StringSet",
        "llvm::DenseSet", "llvm::SparseSet", "llvm::SparseMultiSet",
        "llvm::FoldingSet",	"llvm::SetVector", "llvm::UniqueVector",
        "llvm::ImmutableSet",
    	// Map-like containers
    	"llvm::StringMap", "llvm::IndexedMap", "llvm::DenseMap",
        "llvm::ValueMap", "llvm::IntervalMap", "llvm::MapVector",
        "llvm::IntEqClasses", "llvm::ImmutableMap",
    	// Bit storage containers
    	"llvm:BitVector", "llvm::SmallBitVector", "llvm::SparseBitVector",
    	"llvm::CoalescingBitVector"
    ),
    ".*"){
        std::cout << "This message means that LLVMContainersAnalysis was successfully registered!\n";
}

//-----------------------------------------------------------------------------

void LLVMContainersAnalysis::processFeatures(nlohmann::ordered_json j){
    if(j.contains(ImplicitClassKey)){
        nlohmann::ordered_json res;
        // Prevalence of LLVM containers
        templatePrevalence(j.at(ImplicitClassKey), res);
        Statistics[key] = res;
    }
}

//-----------------------------------------------------------------------------
