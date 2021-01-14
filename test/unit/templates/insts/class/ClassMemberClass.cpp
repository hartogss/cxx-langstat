// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %S/../../../../../build/cxx-langstat --analyses=tia --store %t1.ast --
// RUN: diff %t1.ast.json %s.json

// What about instantiations of member class templates?
//
//

class C {
    template<typename T>
    class D {
        T t;
    };
public:
    D<float> d;
};

template class C::D<int>;
C c;
