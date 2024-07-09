.PHONY: all clean test build_test

testname := $(wordlist 2, 2, $(MAKECMDGOALS))

all:
	+$(MAKE) -C src
	cp src/kcomp .

clean:
	+$(MAKE) -C src clean
	+$(MAKE) -C test clean
	rm -f kcomp

intermediate_test: all
	./kcomp intermediate_test/$(testname).k 2> $(testname).ll
	clang++ -o $(testname) $(testname).ll intermediate_test/call$(testname).cpp
	./$(testname)
	rm -f $(testname) $(testname).ll

test: all
	+$(MAKE) -C test $(testname)
	test/$(testname)
	rm -f $(testname) $(testname).ll

%::
	@true