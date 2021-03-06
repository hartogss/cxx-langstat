// RUN: rm %t1.ast.json || true
// RUN: %clangxx %s -emit-ast -o %t1.ast
// RUN: %cxx-langstat --analyses=cla -emit-features -in %t1.ast -out %t1.ast.json --
// RUN: diff %t1.ast.json %s.json
// Test to check that only standard library vector is matched
//
//

#include <vector>


namespace n {
    template<typename T>
    class vector {

    };
}

int main(int argc, char** argv){
    n::vector<bool> badvec;
    std::vector<int> ivec;
    using namespace std;
    vector<double> dvec;
}
