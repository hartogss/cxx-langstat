void callee1(){ //functionDecl(hasBody(compoundStmt(hasDescendant(forStmt()))))
    for(;;){
    }
}

void callee2(){ //functionDecl(hasBody(compoundStmt(hasDescendant(forStmt()))))
    if(true){
        for(;;){
        }
    }
}

void callee3(){ //functionDecl(hasBody(compoundStmt(hasDescendant(forStmt()))))
    if(true){

    }
    for(;;){
    }
}

void callee4(){ //functionDecl(hasBody(compoundStmt(hasDescendant(forStmt()))))
    if(true){

    }
    callee1();
    for(;;){
    }
}


void caller(){ //functionDecl(hasBody(compoundStmt(hasDescendant(forStmt()))))
    for(;;){        // forStmt(hasDescendant(callExpr()))
        callee1();
    }
}



int main(int argc, char** argv){
}
