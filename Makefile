myls: myls.c
	gcc -Wall -pedantic -g -o myls myls.c

.PHONY: clean
clean:
	rm -f myls
