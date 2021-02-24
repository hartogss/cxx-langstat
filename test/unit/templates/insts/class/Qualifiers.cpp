// RUN: rm %t1.ast.json || true
// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %cxx-langstat --analyses=tia -emit-features -in %t1.ast -out %t1.ast.json --
// RUN: diff %t1.ast.json %s.json

// Right now, qualifiers will not be stripped away from the template type
// arguments. I.e. there is one use of Widget1<const int>, and 2
// uses of Widget1<int>.

template<typename T>
class Widget1 {

};

Widget1<int> w1;
Widget1<const int> w2;
const Widget1<int> w3;
