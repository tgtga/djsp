OBJ_DIR := src
EXE_DIR := .
BUILD_DIR := build

EXE_SRC := $(wildcard *.c)
EXE_OBJ := $(EXE_SRC:%.c=$(BUILD_DIR)/%.o)
EXE     := $(EXE_SRC:%.c=%)
OBJ_SRC := $(wildcard $(OBJ_DIR)/*.c)
OBJ     := $(OBJ_SRC:%.c=$(BUILD_DIR)/%.o) 

# $(info EXE_SRC: $(EXE_SRC))
# $(info EXE_OBJ: $(EXE_OBJ))
# $(info EXE:     $(EXE))
# $(info OBJ_SRC: $(OBJ_SRC))
# $(info OBJ:     $(OBJ))

CC := clang

# for the future: add 'override' at the beginning
# if you want to append to a variable set at invocation
CFLAGS += \
	-std=c99 \
  -O3 -march=native -mtune=native \
  -Wall -Wextra -Wpedantic
LDFLAGS := -lgmp

all: $(EXE)

$(EXE): %: libdjsp.so $(BUILD_DIR)/%.o
	$(CC) $(LDFLAGS) $(LOADLIBES) $(LDLIBS) $^ -o $@

libdjsp.so: $(OBJ)
	$(CC) -shared $^ -o $@

$(BUILD_DIR)/$(OBJ_DIR)/%.o: $(OBJ_DIR)/%.c | $(BUILD_DIR)/$(OBJ_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -fPIC -c $< -o $@
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)/$(EXE_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(BUILD_DIR)/$(OBJ_DIR) $(BUILD_DIR)/$(EXE_DIR): $(BUILD_DIR)
	mkdir -p $@
$(BUILD_DIR):
	mkdir -p $@

clean:
	-rm -r $(BUILD_DIR) libdjsp.so $(EXE)

.PHONY: all clean