#include <vector>
#include <array>
#include <map>

// Test with "vector" in different namespace
// None should match
namespace n {
    struct vector {
        int x;
    };
    struct derivedVector : vector {

    };
} // namespace n
// Test with std::vector parameter variable declaration
int parmvarfunc(std::vector<int> vec){
    return 0;
}
// Returning directly does not match
// Might change, because returning declared variable would match
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

    using namespace std;
    array<float, 3> badarr;
    array<std::vector<int>, 3> badarr2;
    vector<bool> badvec;

    // Anti-examples
    n::vector pseudovec;
    n::derivedVector pseudovec2;
    int x;
}
// Template variable, should match.
template<typename T>
std::array<T, 2> Tarr;
// Template function, local variable should match.
template<typename T>
int func3(){
    std::vector<T> Tvec;
    return 0;
}

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
// - Qualifiers? access specifiers? static? extern? Don't care, all stripped
//   away.
// - Explicit use of 'struct' keyword, namespaces? ignored by converting
//   ElaboratedType to TemplateSpecializationType first if necessary.
// - What about templates? std::vector<T>? Ideally, returns that there is a
//   vector and reports that one of them had template type. Still problem with
//   returning base type.
// - Field decls? Indirect field decls? Reported separately.
// - What about references defined through structured bindings? Not relevant,
//   because it is definition, not declaration?
// - What about returning standard library type literals (e.g. return
//   std:vector<int>{1,2}) ?

// Fundamental question: do we match decls with certain types or do we match
// types directly?
