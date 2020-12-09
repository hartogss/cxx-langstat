// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %S/../../build/cxx-langstat %t1.ast --
// RUN: diff %t1.ast.json %s.json

// Test what happens when instantiating function templates that instantiate
// classes inside them. The subtle point is that we don't want to count class
// instantiations that are inside function templates. We only count them inside
// the instantiation function template.

template<typename T>
class Widget1 {

};

// Function template 1
template<typename T>
void f1(){
    Widget1<T> w1;
}
template void f1<int>();
void caller1(){
    f1<int>(); // causes f1<int>, Widget1<int>
    f1<int>(); // doesn't cause extra insts
    f1<char>(); // causes f1<char>, Widget1<char>
}

// Function template 2
template<typename T>
void f2(){
    Widget1<float> w1; // without the function instantiation this instantiation is not recorded
}                      // we don't want 3rd widget<float> instance
template void f2<int>(); // causes f2<int>, Widget1<float>
template void f2<char>(); // causes f2<char>, Widget1<float>
