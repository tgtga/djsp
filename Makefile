SRC_DIR := src
OBJ_DIR := obj

SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
EXE := djsp

CC := clang
CFLAGS := \
  -O3 -march=native -mtune=native \
	-Weverything -Wno-format-nonliteral -Wno-unreachable-code-break
LDFLAGS := -lgmp

all: $(EXE)

$(EXE): $(OBJ_DIR)/djsp.o $(EXE).so
	$(CC) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

$(EXE).so: $(OBJ)
	@ar rsv $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@
$(OBJ_DIR)/djsp.o: djsp.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $@

clean:
	-rm -r obj $(EXE).so $(EXE)

.PHONY: all clean
