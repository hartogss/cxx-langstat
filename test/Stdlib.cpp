#include <vector>
#include <array>

namespace n {
    struct vector {
        int x;
    };
    struct dervector : vector {

    };
} // namespace n

int main(int argc, char** argv){
    std::vector<int> ivec;
    n::vector pseudovec;
    n::dervector pseudovec2;
    std::vector<double> dvec;
    volatile std::array<int, 4> i4arr;

    // vector<float> badvec;

    using namespace std;
    array<float, 3> badarr;

    // Do we include parameter variable decls?
    // Do we include variable template decls?
    // declarations vs definitions, how do we count them?
    // what about pointer to standard library types?
    // qualifiers? access specifiers? static? extern?

}
