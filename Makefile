SRC_DIR := src

SRC := $(wildcard $(SRC_DIR)/*.c)
EXE := djsp

CC := clang
CFLAGS := \
  -O3 -march=native -mtune=native \
	-Weverything -Wno-format-nonliteral -Wno-unreachable-code-break
LDFLAGS := -lgmp

all: $(EXE)

$(EXE): djsp.c lib$(EXE).so
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $@ $^ $(LOADLIBES) $(LDLIBS) 

lib$(EXE).so: $(SRC)
	$(CC) $(CFLAGS) $(CPPFLAGS) -fPIC -shared -o $@ $^

clean:
	-rm -r lib$(EXE).so $(EXE)

.PHONY: all clean
