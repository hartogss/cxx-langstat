// RUN: rm %t1.ast.json || true
// RUN: %clangxx %s -emit-ast -o %t1.ast -std=c++14
// RUN: %cxx-langstat --analyses=msa -emit-features -in %t1.ast -out %t1.ast.json -- -std=c++14
// RUN: diff %t1.ast.json %s.json

// construction is elidable prior to C++17. -> report construction kind "move" in
// cxx14.cpp.json. Elision in C++14 not yet mandated by standard.
// MSA improvement: report if elidable or not.

class C {

};

void f(C c);

int main(int argc, char** argv){
    f(C());
}
