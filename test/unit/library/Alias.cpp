// RUN: rm %t1.ast.json || true
// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %cxx-langstat --analyses=cla -emit-features -in %t1.ast -out %t1.ast.json --
// RUN: diff %t1.ast.json %s.json

// Test whether instantiations of typedef/alias templates are reported.
//
//

#include <vector>

// alias template
template<typename T>
using v = std::vector<T>;

// 'typedef template' !
template<typename T>
struct vprime{
    typedef std::vector<T> type;
};

void func(){
    v<long> lvec;
    vprime<long>::type lvec2;
}


// Explicit isntantiations of alias templates does not seem to be possible with
// clang
// template class v<int>;
// and doesn't have any effect with "typedef templates". Which makes sense,
// since this instantiation doesn't cause any std::vector<int> to exist as a
// variable.
template class vprime<int>;
