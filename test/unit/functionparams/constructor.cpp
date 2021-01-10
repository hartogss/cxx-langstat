// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %S/../../../build/cxx-langstat --analyses=msa --store %t1.ast --
// RUN: diff %t1.ast.json %s.json

// Test to ensure that constructors' and assignment operators' parameters
// are never looked at.
//

class C{
    C()=default;
    C(int a){
    }
    C(const C& c)=default;
    C(C&& c)=default;
    ~C()=default;
};

class D{
    D()=default;
    ~D()=default;
    void operator=(const D& d);
};
