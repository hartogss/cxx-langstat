//1
struct simple {
    int x;
};

typedef simple s_one; // simple typedef
using s_two = simple; // simple alias

//2
template<typename T>
struct simple2 {
    T x;
};
// typedef
using s2_two = simple2<int>; // also simple alias
template<typename T>
using s2_two2 = simple2<T>; // alias template

int main(int argc, char** argv){

}
