objects := intmath.o logging.o seq.o djsp.o

CC := clang
CFLAGS += -Wall -Wunreachable-code -O3
LDFLAGS += -lgmp

djsp: $(objects)

$(objects): %.o: %.c

.PHONY: clean
clean:
	-rm djsp $(objects)
