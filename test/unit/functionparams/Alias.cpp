// RUN: rm %t1.ast.json || true
// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %S/../../../build/cxx-langstat --analyses=fpa -emit-features -in %t1.ast -out %t1.ast.json --
// RUN: diff %t1.ast.json %s.json

// Short test to ensure that param types are reported as their desugared,
// canonical types. Function signatures are still reported as sugared types.
//

class C{};

using D = C;

void f1(D c);
void f1(const D c);

void func(int a[1]);
