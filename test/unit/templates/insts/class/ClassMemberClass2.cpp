// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %S/../../../../../build/cxx-langstat --analyses=tia -emit-features -in %t1.ast -out %t1.ast.json --
// RUN: diff %t1.ast.json %s.json

// What about instantiations of member class templates?
// Not much difference to any ClassInstClass test, really.
// Again only one C<int>::D<int> instantiation because d is a member variable.

template<typename U>
class C {
    template<typename T>
    class D {
        T t;
    };
public:
    D<U> d;
};

C<int> c;
C<int> c2;
