// RUN: rm %t1.ast.json || true
// RUN: %clangxx %s -emit-ast -o %t1.ast -std=c++14
// RUN: %cxx-langstat --analyses=msa -emit-features -in %t1.ast -out %t1.ast.json -- -std=c++14
// RUN: diff %t1.ast.json %s.json

// MSA has not (yet) the ability to analyze if returned values are copied, moved
// or elided.
//


class C {

};

C f(){
    return C();
}

int main(int argc, char** argv){
    C c = f();
}
