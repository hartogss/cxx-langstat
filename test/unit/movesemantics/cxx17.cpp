// RUN: rm %t1.ast.json || true
// RUN: clang++ %s -emit-ast -o %t1.ast -std=c++17
// RUN: %cxx-langstat --analyses=msa -emit-features -in %t1.ast -out %t1.ast.json -- -std=c++17
// RUN: diff %t1.ast.json %s.json

// construction is elided in C++17. -> report construction kind "unknown" in
// cxx17.cpp.json
// MSA improvement: report something better than "unknown".

class C {

};

void f(C c);

int main(int argc, char** argv){
    f(C());
}
