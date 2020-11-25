// #include <iostream>

//-----------------------------------------------------------------------------

// Old way Nr. 1:
// 'data' should be static because everybody using this should see the same?
template<typename T>
class s {
public:
    static T data;
};
template<>
int s<int>::data = 42;
// template<>
// int s3<double>::data = 41.0;

//-----------------------------------------------------------------------------

// Old way Nr. 2:
// Constexpr function template (CFT)
template<typename T>
constexpr T f1(){
    T data = 3;
    return data;
}
// Not CFT as not constexpr
template<typename T>
T f2(){
    T data = 3;
    return data;
}
// Not CFT as not templated
constexpr int f3(){
    int data = 3;
    return data;
}
// Probably shouldn't be CFT since it does not return the data it defines
template<typename T>
constexpr T f4(){
    T data = 3;
    return 0;
}
// Shouldn't be CFT, doesn't define anything
template<typename T>
constexpr T f5(){
    return 0;
}
// Not CFT, doesn't return
template<typename T>
constexpr T f6(){
    T data = 3;
}
// Not CFT, has function call
template<typename T>
constexpr T f7(){
    T data;
    f1<T>();
    return data;
}
// Not CFT, returns something different that data (irrelevant that it's a call)
template<typename T>
constexpr T f8(){
    T data;
    return f1<T>();
}
// Not CFT, contain declStmt that contains typedef
template<typename T>
constexpr T f9(){
    T data;
    typedef T T_prime;
    return data;
}
// Debatable.
template<typename T>
constexpr T f10(){
    T data;
    T data_prime;
    return data;
}


// Now: variable templates (since C++14)
template<typename T>
constexpr T pi = T(3.14); // what does T() do?

class s2 {
    template<typename T>
    static constexpr T pi2 = T(3);
    // void method(){
    //     template<typename T>
    //     static constexpr T pi2 = T(3);
    // }
    // constexpr int t = 3;
};

int main(int argc, char** argv){
    s<int> s1;
    s1.data = 3;
    s<int> s2;
    pi<double>;
    pi<int>;
    int x = 3;
    // std::cout << s<int>::data << std::endl;
    // std::cout << s<double>::data << std::endl;
    // std::cout << s1.data << std::endl;
    // std::cout << s2.data << std::endl;

}
