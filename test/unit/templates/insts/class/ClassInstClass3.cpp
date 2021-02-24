// RUN: rm %t1.ast.json || true
// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %cxx-langstat --analyses=tia -emit-features -in %t1.ast -out %t1.ast.json --
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
