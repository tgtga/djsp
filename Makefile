SRC_DIR := src
OBJ_DIR := obj

SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

CC := clang
CFLAGS := \
	-std=c99 \
  -O3 -march=native -mtune=native \
	-Weverything -Wno-format-nonliteral -Wno-unreachable-code-break
LDFLAGS := -lgmp

all: djsp

djsp: $(OBJ_DIR)/djsp.o libdjsp.so
	$(CC) $(LDFLAGS) $(LOADLIBES) $(LDLIBS) $^ -o $@

libdjsp.so: $(OBJ)
	$(CC) -shared $^ -o $@

$(OBJ_DIR)/djsp.o: djsp.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -fPIC -c $< -o $@

$(OBJ_DIR):
	mkdir -p $@

clean:
	-rm -r obj libdjsp.so djsp

.PHONY: all clean