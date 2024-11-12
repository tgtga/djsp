SRC_DIR := src
OBJ_DIR := obj

SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
EXE := djsp

CC      := clang
CFLAGS  := -Wall -Wunreachable-code -O3
LDFLAGS := -lgmp

all: $(EXE)

$(EXE): $(OBJ) $(OBJ_DIR)/djsp.o
	$(CC) -o $@ $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@
$(OBJ_DIR)/djsp.o: djsp.c | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $@

clean:
	-rm -r obj $(EXE)

.PHONY: all lib clean
