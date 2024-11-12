objects := intmath.o logging.o seq.o

CC := clang
CFLAGS += -Wall -Wunreachable-code -fPIE -fcommon

%.o: %.c
	$(CC) $(CFLAGS) $< -c -o $@

djsp: $(objects) djsp.o
	$(CC) djsp.o -o djsp -lc -lgmp $(objects)

.PHONY: clean
clean:
	-rm djsp $(objects)
