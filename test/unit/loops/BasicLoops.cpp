// RUN: rm %t1.ast.json || true
// RUN: clang++ %s -emit-ast -o %t1.ast
// RUN: %S/../../../build/cxx-langstat --analyses=lda,lka -emit-features -in %t1.ast -out %t1.ast.json --
// RUN: diff %t1.ast.json %s.json
// Test to ensure LDA finds corrects loop depths and only recognizes top-level
// loops as relevant
// Run lKA too because why not

void loop(){
    for (;;){ //depth 1

    };
}

void loop2(){
    for (;;){ //depth 2
        for (;;){
        }
    }
}

void loop21(){
    for (;;){ //depth 2
        for (;;){
        }
    }
}

void loop3(){
    for (;;){ //depth 3
        for (;;){
            for (;;){
            }
        }
    }
}

void loop4(){
    for (;;){ //depth 4
        for (;;){
            for (;;){
                for (;;){
                }
            }
        }
    }
}

class Myclass {
public:
    void method(){
        for(;;){ //depth 1
        }
    }
    void method2(){
        while(true){ //depth 1

        }
    }
    void method3(){
        do{ //depth 1

        }while(false);
    }
};

void loop5(){
    while(true){ //depth 1

    }
    do{ //depth 1

    }while(true);
}

void loop6(){
    while(true){ //depth 3
        for(;;){
            do{

            }while(true);

        }
    }
}

void loop7(){
    while(true){ //depth 4
        for(;;){
            while(true){
                do{

                }while(false);
            }
        }
    }
}

void loop8(){
    int arr[] = {1,2};
    for(auto i : arr){ //depth 2
        for(;;){

        }
    }
}



int main(int argc, char** argv){
    return 0;
}
