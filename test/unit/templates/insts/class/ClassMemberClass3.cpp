// RUN: rm %t1.ast.json || true
// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %S/../../../../../build/cxx-langstat --analyses=tia -emit-features -in %t1.ast -out %t1.ast.json --
// RUN: diff %t1.ast.json %s.json

// What happens when instantiationg a class template that contains a member
// class?
//

template<typename T>
class C {
    class D {
    };
};

template class C<int>;
