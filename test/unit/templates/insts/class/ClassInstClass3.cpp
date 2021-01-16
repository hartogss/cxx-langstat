// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %S/../../../../../build/cxx-langstat --analyses=tia --out Output/ %t1.ast --
// RUN: diff %t1.ast.json %s.json

// Instantiations: Super<int>, Super<int> as VarDecls.
// Sub<int> only once, because it exists once as FieldDecl in the
// ClassTemplateSpecializationDecl in the AST part of Super<int>.

template<typename T>
class Sub {};

template<typename T>
class Super {
    Sub<T> w;
};

Super<int> s1;
Super<int> s2;
