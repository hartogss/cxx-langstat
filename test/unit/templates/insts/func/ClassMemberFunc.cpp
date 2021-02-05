// RUN: rm %t1.ast.json || true
// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %S/../../../../../build/cxx-langstat --analyses=tia -emit-features -in %t1.ast -out %t1.ast.json --
// RUN: diff %t1.ast.json %s.json
// Instantiations: C::fc<int>, f<bool>, D<bool>.
// Question: Do we want to report the instantiation D<bool>::fd ?
//


// Here the class doesn't actually instantiate, we instantiate the method
// explicitly.
class C {
public:
    template<typename T>
    void fc(){}
};
void callerc(){
    C c;
    c.fc<int>();
}


// Non-templated methods of class templates, however, are "instantiated" when
// the class template is instantiated.
// I've decided not to report function call, because they don't have any template
// parameters themselves, and thus really nothing interesting to report.
template<typename T>
struct D {
    void fd(){}
};
void callerd(){
    D<int> d;
    d.fd();
}

// Same as ClassInstFunc: explicit instantiation of class does not instantiate
// member (in this case a member function)
template<typename T>
class E {
public:
    template<typename U>
    void fe(){}
    template<typename R>
    static void fe2(T t, R r){}
};
template class E<bool>;

// Note how the method instantiation does not cause an instantiation of the
// encompassing class.
void caller(){
    E<bool> e;
    e.fe<bool>();

    E<int> e1;
    e1.fe2(3,3); // causes ::fe2<int> to be deduced
    E<int>::fe2<short>(3, 3); // om het af te leren
}
