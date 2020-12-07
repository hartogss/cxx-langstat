//-----------------------------------------------------------------------------
// Variable template 1
// Since C++20 this could also be a floating point non-type template parameter
template<int N>
int data = N;
// Explicit instantiation
template int data<0>;
// Implicit instantiations
int caller(){
    data<1>;
    return data<0>;
}

// Variable template 2
template<int N>
int data2 = N;
int caller2(){
    return data2<42>;
}

// Variable template 3
// https://en.cppreference.com/w/cpp/language/variable_template
template<typename T>
constexpr T pi = T(3.1415926535897932385L);
// Implicit instantiation
void caller3(){
    pi<float>;
    pi<double>;
    pi<double>;
    int test = pi<int> + pi<int>;
}

// Variable template 4
template<int N, int M, typename T>
int data4 = N;
template int data4<3, 42, double>;
