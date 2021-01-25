// RUN: rm %t1.ast.json || true
// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %S/../../../build/cxx-langstat --analyses=sla2 -emit-features -in %t1.ast -out %t1.ast.json --
// RUN: diff %t1.ast.json %s.json

// Test whether alias/typedef are matched. They're not. This is not really
// wrong, but something that could be improved for SLA/TIA.
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
    vprime<long> lvec2;
}
