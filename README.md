# cxx-langstat

## Building
Requires at least LLVM 11 with clang, clang-tools-extra.  
To build, clone project into directory:  
`mkdir build && cd build`  
`cmake -DCMAKE_CXX_COMPILER=your/clang++/binary/here ../`  
Usually located in '/usr/local/bin/clang++'  

## Running
To run:  
` ./cxx-langstat [options] <source0> [... <sourceN> ]`  
Options:  
`--forstmt <int>` Maximum depth for which loop statistics will be calculated. Defaults to 0.
