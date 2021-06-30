// RUN: rm %t1.ast.json || true
// RUN: %clangxx %s -emit-ast -o %t1.ast
// RUN: %cxx-langstat --analyses=tia -emit-features -in %t1.ast -out %t1.ast.json --
// RUN: diff %t1.ast.json %s.json
// Instantiations: f<int>, f<bool>, E<short>, F<unsigned>, fK<unsigned>.
// Test to see how classes declarations or class instantiations cause function
// instantiations.

template<typename T>
T f();

// Class implicitly instantiations function through call
class C {
    void fc(){
        f<void>();
    }
};

// Class implicitly insts through call to create var member
class D {
    bool a = f<bool>();
};

// f<long> instantiated regardless if Dt instantiated or not, same as above
template<typename T>
class Dt {
    long a = f<long>(); // no clue why this gives 1 as loc 
};

// Explicit class instantiation does not seem to instantiate members...
// no f<short>
template<typename T>
class E {
    T a = f<T>();
    static int a2;
};
template class E<short>;
// According to https://en.cppreference.com/w/cpp/language/class_template,
// a static data member of a class template should be explicitly instantiable,
// how?

// Implicit class inst does however, which makes sense because we really need
// an E object here
E<unsigned> e;
