// Typedef & alias
struct tuple_int {
    int x;
    int y;
};

typedef tuple_int pair_typedef; // typedef
using pair_alias = tuple_int; // alias
pair_typedef simpletest;

//-----------------------------------------------------------------------------

// "typedef templates" & alias templates
template<typename T>
struct tuple {
    T x;
    T y;
};

// typedef & alias
typedef tuple<int> intpair_typedef; //typedef
using intpair_alias = tuple<int>; // alias

// Best example of a "typedef template". typedef public by default
template<typename T>
struct Tpair_typedef {
    typedef tuple<T> type;
};

// Not a "typedef template" since it contains no tyedef
template<typename T>
struct empty {
};

// Not a "typedef template" because it contains a variable declaration
template<typename T>
struct Tpair_typedef2 {
    typedef tuple<T> type;
    int x;
};

// Not a "typedef template" because it contains a method declaration
template<typename T>
struct Tpair_typedef3 {
    typedef tuple<T> type;
    void method();
};

// "typedef template" since access specifier is public
template<typename T>
class Tpair_typedef4 {
public:
    typedef tuple<T> type;
};
Tpair_typedef4<int>::type test;

// Not considered "typedef template" since only itself and derived classes can
// use it.
// Note that then the typedef contained is then counted as a regular typedef.
template<typename T>
class Tpair_typedef5 {
protected:
    typedef tuple<T> type;
};
template<typename T>
class der : public Tpair_typedef5<T> {
    Tpair_typedef5<int>::type tuple;
};

// Depends on perspective. For now, I don't consider private typedefs to be
// "typedef templates", since they can only be used by other typedefs in the
// class template. Any other statement would cause it not to be a "typedef
// template" anymore.
// Note that then the typedef contained is then counted as a regular typedef.
template<typename T>
class Tpair_typedef6 {
private:
    typedef tuple<T> type;
public:
    typedef Tpair_typedef6<T>::type type2;
};
Tpair_typedef6<float>::type2 test6;

// alias template
template<typename T>
using Tpair_alias = tuple<T>;
Tpair_alias<int> anothertest;


int main(int argc, char** argv){

}
