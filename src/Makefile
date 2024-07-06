LLVM16_INCLUDE_PATH = "/opt/homebrew/opt/llvm@16/include/"

.PHONY: clean all

all: kcomp

kcomp: driver.o parser.o scanner.o kcomp.o
	clang++ -o kcomp driver.o parser.o scanner.o kcomp.o `llvm-config --cxxflags --ldflags --libs --libfiles --system-libs`

kcomp.o:  kcomp.cpp driver.hpp
	clang++ -c kcomp.cpp -I$(LLVM16_INCLUDE_PATH) -std=c++17 -fno-exceptions -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS
	
parser.o: parser.cpp
	clang++ -c parser.cpp -I$(LLVM16_INCLUDE_PATH) -std=c++17 -fno-exceptions -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS
	
scanner.o: scanner.cpp parser.hpp
	clang++ -c scanner.cpp -I$(LLVM16_INCLUDE_PATH) -std=c++17 -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS 
	
driver.o: driver.cpp parser.hpp driver.hpp
	clang++ -c driver.cpp -I$(LLVM16_INCLUDE_PATH) -std=c++17 -fno-exceptions -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS 

parser.cpp, parser.hpp: parser.yy 
	bison -o parser.cpp parser.yy

scanner.cpp: scanner.ll
	flex -o scanner.cpp scanner.ll

clean:
	rm -f *~ driver.o scanner.o parser.o kcomp.o kcomp scanner.cpp parser.cpp parser.hpp
