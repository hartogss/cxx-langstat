# cxx-langstat

cxx-langstat is a clang-based tool to analyze the adoption and prevalence of language features in C/C++ codebases.
It uses clang's ASTMatchers library in combination with LibTooling to analyze user code on the Abstract Syntax Tree (AST) level.

## Building
Requires at least LLVM 11 with clang, clang-tools-extra.  
To build, clone project into directory:  
`mkdir build && cd build`  
`cmake -DCMAKE_CXX_COMPILER=your/clang++/binary/here ../`  
Usually located in `/usr/local/bin/clang++`.   

## Running
To run:  ` ./cxx-langstat [options] <source0> [... <sourceN> ]`  
Options:  Currently, there are not options. Might change in the future.


## Implemented Analyses
### Cyclomatic Complexity (CCA)
For each explicit (not compiler-generated) function declaration that has a body (i.e. is defined), calculates the so-called cyclomatic complexity. This concept developed by Thomas J. McCabe, intuitively, computes for a "section of source code the number of independent paths within it, where linearly-independent means that each path has at least one edge that is not in the othe paths." (https://en.wikipedia.org/wiki/Cyclomatic_complexity)
### Loop Depth (LDA)
LDA computes statistics about loop depths, i.e., computes how deeply nested loops are. Example of depth 2:
```c++
for(;;){
  do{
  // Are you gonna do something useful here?
  }while(true);
}
```
Currently the matchers for this analysis grow exponentially with the maximum loop depth to look for, which is not (yet) a problem since depths >5 are rare. Still, switching to a dominator tree-based approach might be favorable.
### Loop Kind (LKA)
Computes statistics on usage `for, while, do-while` and range-based `for` loops in C++. Especially interesting to use to see the adoption of range-based `for` since C++11.
### Standard Library Container Usage (SLA)
Counts occurences of standard library containers `array, vector, forward_list, list, map, multimap, set, multiset, unordered_map, unordered_multimap, unordered_set, unordered_multiset, queue, prioity_queue, stack, deque` as variables or record member fields and for each type outputs what types they contain.
### `using` (UA)
C++11 introduced type aliases (`using` keyword) which are similar to `typedef`s, but additionally can be templated. This analysis aims to find out if programmers shifted from typedefs to aliases, possibly due to the fact that aliases can be templated. The analysis gives you usage figures of typedefs, aliases, "typedef templates" (an idiom used to get around above said typedef limitation) and alias templates.
### WIP: Variable Templates (VTA)
