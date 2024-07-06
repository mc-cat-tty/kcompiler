.PHONY: all

all:
	+$(MAKE) -C src
	cp src/kcomp .

clean:
	+$(MAKE) -C src clean
	rm kcomp