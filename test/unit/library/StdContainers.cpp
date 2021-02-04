// RUN: rm %t1.ast.json || true
// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %S/../../../build/cxx-langstat --analyses=cla -emit-features -in %t1.ast -out %t1.ast.json --
// RUN: diff %t1.ast.json %s.json
// Some old test I still wanted to run to check for library container usage.
//
//

#include <vector>
#include <array>
#include <map>
#include <set>
#include <unordered_set>

// Test with "vector" in different namespace
// None should match
namespace n {
    struct vector {
        int x;
    };
    struct derivedVector : vector {

    };
    template<typename T, T x>
    class test {
        T n=x;
    };
} // namespace n
// Test with std::vector parameter variable declaration
int parmvarfunc(std::vector<int> vec){
    return 0;
}
// Returning literals does not match
// Feature one might add in the future
std::vector<int> func2(){
    return std::vector<int>{1,2};
}
void simplefunc(){
    // Prototypical examples
    std::vector<int> intvec;
    std::array<int, 2> intarr;
    const std::array<int, 1> cintarr = {1};
    std::vector<double> dvec;
    dvec = {2.0}; // definition has no effect
    volatile std::array<int, 4> i4arr;
    // vector<float> badvec; // will not work because of missing namespace qual.
    std::map<float, n::vector> wmap;

    using namespace std;
    array<float, 3> badarr;
    array<std::vector<int>, 3> badarr2;
    vector<bool> badvec; // Reported in a special way because of bool wrappers

    // Anti-examples, because not templates
    n::vector pseudovec;
    n::derivedVector pseudovec2;
    int x;

    n::test<int, 3> t1;
    // n::test t2 = 4; //CTAD?
}
template class n::test<char, 'a'>;

// Template variable, doesn't match
template<typename T>
std::array<T, 2> Tarr;
// Template function, local variable shouldn't match.
template<typename T>
int func3(){
    std::vector<T> Tvec;
    return 0;
}
template<typename ...Ts>
std::unordered_set<Ts...> Tsset;

// Field declarations should match too, but be reported separately.
struct s {
    std::vector<int> v1;
};
struct s1 {
    struct {
        std::vector<int> v1;
    };
};
template<typename T>
class c1 {
    std::map<T, int> m1;
};

int main(int argc, char** argv){

}


// - Do we include parameter variable decls? Yes
// - Do we include variable template decls?
// - Declarations vs definitions, how do we count them? We count only decls,
//   as those are given by decl().
// - What about pointers to standard library types?
//   Not counted for now.
// - Qualifiers? access specifiers? static? extern? Don't care, all stripped
//   away.
// - What about templates? std::vector<T>? Ideally, returns that there is a
//   vector and reports that one of them had template type. Still problem with
//   returning base type.
// - Field decls? Indirect field decls? Reported separately.
// - What about references defined through structured bindings? Not relevant,
//   because it is definition, not declaration?
// - What about returning standard library type literals (e.g. return
//   std:vector<int>{1,2}) ?
//   Not done for now.

// Fundamental question: do we match decls with certain types or do we match
// types directly?
// We match declarations that have certain standard library types to be able
// to count those declarations s.t. we know how those library types are used.
