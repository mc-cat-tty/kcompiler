.PHONY: all clean test

testname := $(wordlist 2, 2, $(MAKECMDGOALS))

all:
	+$(MAKE) -C src
	cp src/kcomp .

clean:
	+$(MAKE) -C src clean
	rm -f kcomp

test: all
	./kcomp test/$(testname).k 2> $(testname).ll
	clang++ -o $(testname) $(testname).ll test/call$(testname).cpp
	./$(testname)
	rm -f $(testname) $(testname).ll

%::
	@true