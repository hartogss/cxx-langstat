//1
struct tuple {
    int x;
    int y;
};

typedef tuple pair_typedef; // typedef
using pair_alias = tuple; // alias


//2
template<typename T>
struct tuple2 {
    T x;
    T y;
};

// typedef & alias
typedef tuple2<int> intpair_typedef; //typedef
using intpair_alias = tuple2<int>; // alias

// Best example of a "typedef template". typedef public by default
template<typename T>
struct Tpair_typedef {
    typedef tuple2<T> type;
};
// Not a "typedef template" since it contains no tyedef
template<typename T>
struct empty {
};
// Not a "typedef template" because it contains a variable declaration
template<typename T>
struct Tpair_typedef2 {
    typedef tuple2<T> type;
    int x;
};
// Not a "typedef template" because it contains a method declaration
template<typename T>
struct Tpair_typedef3 {
    typedef tuple2<T> type;
    void method();
};
// "typedef template" since access specifier is public
template<typename T>
class Tpair_typedef4 {
public:
    typedef tuple2<T> type;
};
Tpair_typedef4<int>::type test;
// Not considered "typedef template" since only itself and derived classes can
// use it.
template<typename T>
class Tpair_typedef5 {
protected:
    typedef tuple2<T> type;
};
template<typename T>
class der : public Tpair_typedef5<T> {
    Tpair_typedef5<int>::type tuple;
};
// Depends on perspective. For now, I don't consider private typedefs to be
// "typedef templates", since they can only be used by other typedefs in the
// class template. Any other statement would cause it not to be a "typedef
// template" anymore.
template<typename T>
class Tpair_typedef6 {
private:
    typedef tuple2<T> type;
public:
    typedef int type2;
};


// alias template
template<typename T>
using Tpair_alias = tuple2<T>; // alias template



int main(int argc, char** argv){

}
