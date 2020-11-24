#include <iostream>

// Old way Nr. 1:
// 'data' should be static because everybody using this
template<typename T>
class s {
public:
    static T data;
};
template<>
int s<int>::data = 42;
// template<>
// int s3<double>::data = 41.0;

// Old way Nr. 2:
template<typename T>
constexpr T func(){
    T data = 3;
    return data;
}

// Now: variable templates (since C++14)
template<typename T>
constexpr T pi = T(3);

class s2 {
    template<typename T>
    static constexpr T pi2 = T(3);
    // void method(){
    //     template<typename T>
    //     static constexpr T pi2 = T(3);
    // }
};

int main(int argc, char** argv){
    s<int> s1;
    s1.data = 3;
    s<int> s2;
    std::cout << s<int>::data << std::endl;
    std::cout << s<double>::data << std::endl;
    std::cout << s1.data << std::endl;
    std::cout << s2.data << std::endl;

}
