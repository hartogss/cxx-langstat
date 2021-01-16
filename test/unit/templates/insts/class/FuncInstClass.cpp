// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %S/../../../../../build/cxx-langstat --analyses=tia --out Output/ %t1.ast --
// RUN: diff %t1.ast.json %s.json

//
//
//

template<typename T>
class Widget {
};
// # Functions
// Class instantiation through ParmVarDecl
// Widget<bool> only detected when f0 has body
void f0(Widget<bool> w){
}
void f1(){
    Widget<int> w;
}

// # Function templates, but instantiation happens nonetheless without instantiating
// the func template since Widget does not depend on any template type parameter
template<typename T>
void f2(Widget<bool> w){
}
template<typename T>
void f3(){
    Widget<int> w;
}

// # More function templates
template<typename T>
void f6(){
    Widget<T> w;
}
template<typename T>
void f7(){
    Widget<T> w;
}
template void f7<short>();
