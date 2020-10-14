# https://github.com/peter-can-talk/cppnow-2017/blob/master/code/mccabe/Makefile
# https://jonasdevlieghere.com/understanding-the-clang-ast/#astcontext
# https://stackoverflow.com/questions/10024279/how-to-use-shell-commands-in-makefile

TARGET := Langstat
#HEADERS := -isystem /llvm/include/
#WARNINGS := -Wall -Wextra -pedantic -Wno-unused-parameter
#CXXFLAGS := $(WARNINGS) -std=c++14 -fno-exceptions -fno-rtti -O3 -Os
CXXFLAGS := $(shell /usr/local/bin/llvm-config --cxxflags)
LDFLAGS := $(shell /usr/local/bin/llvm-config --ldflags)

CLANG_LIBS := \
	-lclangFrontendTool \
	-lclangRewriteFrontend \
	-lclangDynamicASTMatchers \
	-lclangTooling \
	-lclangFrontend \
	-lclangToolingCore \
	-lclangASTMatchers \
	-lclangParse \
	-lclangDriver \
	-lclangSerialization \
	-lclangRewrite \
	-lclangSema \
	-lclangEdit \
	-lclangAnalysis \
	-lclangAST \
	-lclangLex \
	-lclangBasic

LIBS :=  $(shell /usr/local/bin/llvm-config --libs --system-libs) $(CLANG_LIBS)

all: langstat

.phony: clean
.phony: run

clean:
	rm $(TARGET) || echo -n ""

langstat: $(TARGET).cpp
	clang++ $(TARGET).cpp $(CXXFLAGS) $(LDFLAGS) $(LIBS) -o cxx-langstat
