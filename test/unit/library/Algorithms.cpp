// RUN: rm %t1.ast.json || true
// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %cxx-langstat --analyses=ala -emit-features -in %t1.ast -out %t1.ast.json --
// RUN: diff %t1.ast.json %s.json

// Small test to check usage of Algorithms library. Notice how some std::move
// are move as in move semantics, other are move as in algorithm?
// Make sure that ALA does only capture use of function template from algorithm lib.

#include <algorithm>
#include <utility>
#include <list>
#include <vector>

template<typename T>
void func(){}

int main(int argc, char** argv){
    int a = std::max(3,4);
    func<bool>();
    func<bool>();
    std::min(1,2);
    std::max(3.0,4.0);
    std::max(std::move(3.9), 4.0);


    std::vector<int> v;
    std::list<int> l;
    std::move(v.begin(), v.end(), std::back_inserter(l));

    return std::move(a);

}

// functionDecl(hasName("std::move"), isExpansionInFileMatching("algorithm"))

// callExpr(callee(functionDecl(hasName("std::move"), isTemplateInstantiation())), isExpansionInMainFile())
