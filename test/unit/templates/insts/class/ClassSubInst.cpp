// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %S/../../../../../build/cxx-langstat --analyses=tia -emit-features -in %t1.ast -out %t1.ast.json --
// RUN: diff %t1.ast.json %s.json

//
//
//

template<typename T>
class Widget1 {

};


// Widget1<bool> w_;
// template class Widget1<bool>;

template class Widget1<Widget1<bool>>;
// Widget1<Widget1<bool>> w; // 'Subinstantiation' Widget<bool> not registered -> room for improvement.
