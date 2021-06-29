# cxx-langstat

cxx-langstat is a clang-based tool to analyze the adoption and prevalence of language features in C/C++ codebases.
Leveraging clang's ASTMatchers library we can analyze source code on the AST (abstract syntax tree) level to gain insights about the usage of high-level programming constructs.
By finding the feature/construct in code and counting them, we can achieve insights about the popularity and prevalence of it.

Gaining insights is achieved in two steps, called "stages":
- Emitting features from code: in this stage, the ASTs of the code are considered. By finding all instances of a feature (e.g. all variables that are `constexpr`) we can write a human-readable JSON file that contains all occurrences of a feature that interests us.
- Emitting statistics from features: By using the occurrences from the JSON file from before, we can compute statistics e.g. by counting them.

This separation of the computation of statistics into two steps aids debugging (human-readable features) and avoids recomputation as we can compute new statistics from already extracted features, avoiding reparsing to get the AST or rematching of the AST.

Apart from the analyses that the tool comes with (see below) it also has an API that allows it to register and execute new analyses.

cxx-langstat was developed as part of my Bachelor's thesis at ETH Zurich, see [here](https://www.research-collection.ethz.ch/handle/20.500.11850/480835) for the full text.


## Instructions
### Requirements
- [LLVM](http://llvm.org) (>=11) including clang
- [JSON for Modern C++](https://github.com/nlohmann/json)
- Build system: `ninja` or `make` etc.
- `cmake`

### Building
1. Clone/download cxx-langstat project
2. Download the single-include `json.hpp` from [JSON for Modern C++](https://github.com/nlohmann/json) and put it in `cxx-langstat/include/nlohmann` or use one of the other suggested integration methods
3. `mkdir build && cd build`  
4. `cmake -G "<generator>" -DCMAKE_CXX_COMPILER=<C++ compiler> ../` (`clang++`, `clang++-11` etc.)
5. `ninja` or `make` to build the binary


### Testing
The [LLVM integrated tester](https://www.llvm.org/docs/CommandGuide/lit.html) is used to test if features are correctly extracted from source/AST files - install using pip: `pip install lit` \
When in build directory,  type `lit test -s`. Use `-vv` to see why individual test cases fail.
### Running
Basic usage: ` cxx-langstat [options] `  
Options:
- `-analyses=<string>` Accepts a string of comma-separated shorthands of the analyses, e.g. `-analyses=msa,ula` will run MoveSemanticsAnalysis and UtilityLibAnalysis.
- `-in`, `-indir`: specify input file or directory
- `-out`, `-outdir`: specify output file or directory
- `-emit-features`: compute features from source or AST files
- `-emit-statistics`: compute statistics from features
- `parallel` or `-j`: number of parallel instances to use, works for `-emit-features` only

#### Example use cases:
##### Single file
To analyze a single source or AST file:
1. Extract features: \
`cxx-langstat -analyses=<> -emit-features -in Helloworld.cpp -out Helloworld.cpp.json `
2. Compute statistics: \
`cxx-langstat -analyses=<> -emit-statistics -in Helloworld.cpp.json -out Helloworld.json`

##### Whole project
To analyze a complete software project, specify the root of it using the `-indir` flag
1. `cxx-langstat -analyses=<> -emit-features -indir MyProject/ -outdir features/` \
will automatically consider ALL code files (.cpp, .h, .ast etc.) because of `-emit-features`
2. `cxx-langstat -analyses=<> -emit-statistics -indir features/ -out stats.json`
will automatically consider ALL .json files because of `-emit-statistics`

Emitting features for a project creates a JSON file for each input file, make sure to place them in a directory created before running it.
Computing statistics creates a single JSON file.

### Adding new analyses
A script and instructions for doing will be be merged into main soon.

## Implemented Analyses
###### Algorithm Library Analysis (ALA)
Anecdotal evidence suggests that the [C++ Standard Library Algorithms](https://en.cppreference.com/w/cpp/algorithm) are rarely used, motivating analysis to check this claim.
ALA finds and counts calls to function template from the STL algorithms, however, currently only of the non-modifying sequence operations and the minimum/maximum operations; also, the C++20 `std::ranges` algorithms aren't considered.
###### Constexpr Analysis (CEA)
Computes simple statistics on how often variables, function and if statements are constexpr or not constexpr.
###### Container Library Analysis (CLA)
CLA reports variable declarations whose type is a [C++ Standard Library Container](https://en.cppreference.com/w/cpp/container):
`array`, `vector`, `forward_list`, `list`, `map`, `multimap`, `set`, `multiset`, `unordered_map`, `unordered_multimap`, `unordered_set`, `unordered_multiset`, `queue`, `priority_queue`, `stack`, `deque`.

"Variable declarations" include member variables and function parameters. Other occurrences of containers are not respected.

###### Cyclomatic Complexity Analysis (CCA)
For each explicit (not compiler-generated) function declaration that has a body (i.e. is defined), calculates the so-called cyclomatic complexity. This concept developed by Thomas J. McCabe, intuitively, computes for a "section of source code the number of independent paths within it, where linearly-independent means that each path has at least one edge that is not in the other paths." (https://en.wikipedia.org/wiki/Cyclomatic_complexity)
###### Function Parameter Analysis (FPA)
Computes how often the 4 main kinds of function parameters are used: pass-by value, non-const lvalue reference, const lvalue reference and rvalue reference.
###### Loop Depth Analysis (LDA)
LDA computes statistics about loop depths, i.e., computes how deeply nested loops are. Example of depth 2:
```c++
for(;;){
  do{
  doSomething();
  }while(true);
}
```
Currently the matchers for this analysis grow exponentially with the maximum loop depth to look for, which is not (yet) a problem since depths >5 are rare. Still, switching to a dominator tree-based approach might be favorable.
###### Loop Kind Analysis (LKA)
Computes statistics on usage `for`, `while`, `do-while` and range-based `for` loops in C++. Especially interesting to use to see the adoption of range-based `for` since C++11.
###### Move Semantics Analysis (MSA)
###### Template Parameter Analysis (TPA)
Counts each kind of template (class, function, variable, alias), how many were variadic/use parameter packs and outputs counts on what kind of template parameters were used (non-tupe template parameters, type template parameters and template template parameters).
###### Template instantiation Analysis (TIA)
For the different instantiations (class, function) counts how often these instantations were used. I.e., for class isntantiaitons, counts how many variables and member variables have that type, and for function instantiations, counts how often those instantiations are called through callExprs.
###### `using` Analysis (UA)
C++11 introduced type aliases (`using` keyword) which are similar to `typedef`s, but additionally can be templated. This analysis aims to find out if programmers shifted from `typedef`s to aliases. The analysis gives you usage figures of `typedef`s, aliases, "`typedef` templates" (an idiom used to get around above said `typedef` limitation) and alias templates.
<table>
<tr>
<th> Pre-C++11 </th>
<th> Alias </th>
</tr>
<tr>
<td>

  ```c++
  // Regular typedef
  typedef std::vector<int> IntVector;
  // "typedef template" idiom
  template<typename T>
  struct TVector {
    typedef std::vector<T> type; // doesn't have to be named 'type'
  };
  ```
</td>
<td>

  ```c++
 // alias
 using IntVector = std::vector<int>;
 // alias template
 template<typename T>
 using TVector = std::vector<T>;
  ```
</td>
</tr>
</table>

###### Utility Library Analysis (ULA)
Similar to the Container Library Analysis; analyzes the usage of certain class template types, namely, the following [C++ Utilities](https://en.cppreference.com/w/cpp/utility): `pair`, `tuple`, `bitset`, `unique_ptr`, `shared_ptr` and `weak_ptr`. \
Interesting to extend to see if `auto_ptr` is still used in C++.

###### Variable Template Analysis (VTA)
C++14 added variable templates. Previously, one used either class templates with a static data member or constexpr function templates returning the desired value. We here analyze whether programmers transitioned in favor of the new concept by reporting usage of the three constructs.
<table>
<tr>
<th> Class template with static member </th>
<th> Constexpr function template </th>
<th> Variable template (since C++14) </th>
</tr>
<tr>
<td>

  ```c++
  template<typename T>
  class Widget {
  public:
      static T data;
  };
  ```
</td>
<td>

  ```c++
  template<typename T>
  constexpr T f1(){
      T data;
      return data;
  }
  ```
</td>

<td>

  ```c++
  template<typename T>
  T data;
  ```
</td>
</tr>
</table>
