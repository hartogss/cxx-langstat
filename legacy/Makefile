# https://github.com/peter-can-talk/cppnow-2017/blob/master/code/mccabe/Makefile
# https://jonasdevlieghere.com/understanding-the-clang-ast/#astcontext
# https://stackoverflow.com/questions/10024279/how-to-use-shell-commands-in-makefile
# https://www.youtube.com/watch?v=_r7i5X0rXJk (basic)
# https://www.youtube.com/watch?v=PiFUuQqW-v8 (tracking, pattern rules)

#HEADERS := -isystem /llvm/include/
#WARNINGS := -Wall -Wextra -pedantic -Wno-unused-parameter
#CXXFLAGS := $(WARNINGS) -std=c++14 -fno-exceptions -fno-rtti -O3 -Os

# $@ target, $< first prerequisite, $^ all prerequisites


EXECUTABLE := langstat
TARGET := Langstat

CRUDE := Extraction

# CXXFLAGS := $(shell /usr/local/bin/llvm-config --cxxflags)
CXXFLAGS := -fno-rtti #fno-rtti important why?

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


$(EXECUTABLE): Extraction.o Analysis.o ForStmtAnalysis.o Langstat.o
	clang++ Extraction.o Analysis.o ForStmtAnalysis.o $(TARGET).o $(CXXFLAGS) $(LDFLAGS) $(LIBS) -o $(EXECUTABLE)

$(TARGET).o: $(TARGET).cpp
	clang++ -c $(TARGET).cpp $(CXXFLAGS)

ForStmtAnalysis.o: ForStmtAnalysis.cpp ForStmtAnalysis.h
	clang++ -c ForStmtAnalysis.cpp $(CXXFLAGS)

Analysis.o: Analysis.cpp Analysis.h
	clang++ -c Analysis.cpp $(CXXFLAGS)

Extraction.o: Extraction.cpp Extraction.h
	clang++ -c Extraction.cpp $(CXXFLAGS)

clean:
	rm $(TARGET) *.o

crude: $(CRUDE).cpp $(CRUDE).h
	clang++ $(CRUDE).cpp $(CXXFLAGS) $(LDFLAGS) $(LIBS) -o $(CRUDE)
