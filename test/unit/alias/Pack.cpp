// RUN: rm %t1.ast.json || true
// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %cxx-langstat --analyses=ua -emit-features -in %t1.ast -out %t1.ast.json --
// RUN: diff %t1.ast.json %s.json

// Variadic "typedef templates" and alias templates work with UA?
//
//

template<typename... T>
class C{};

template<typename... T>
using A1 = C<T...>;
A1<int> a1;

template<typename... T>
struct A2 {
    typedef C<T...> type;
};
A2<int, int>::type a2;
