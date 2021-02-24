// RUN: rm %t1.ast.json || true
// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %cxx-langstat --analyses=fpa -emit-features -in %t1.ast -out %t1.ast.json --
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
