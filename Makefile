djsp:
	cc djsp.c -Wall -Wunreachable-code -lgmp -O3 -o djsp

clean:
	rm -f djsp