  OBJ_DIR := src
  EXE_DIR := exe
BUILD_DIR := build

EXE_SRC := $(wildcard $(EXE_DIR)/*.c)
EXE_OBJ := $(OBJ_SRC:%.c=$(BUILD_DIR)/%.o)
EXE     := $(EXE_SRC:$(EXE_DIR)/%.c=%)
OBJ_SRC := $(wildcard $(OBJ_DIR)/*.c)
OBJ     := $(OBJ_SRC:%.c=$(BUILD_DIR)/%.o)

CC := clang

# for the future: add 'override' at the beginning
# if you want to append to a variable set at invocation
CFLAGS := \
	-std=c99 \
  -O3 -march=native -mtune=native \
  -Wall -Wextra -Wpedantic
LDFLAGS := -lgmp

ifdef STATIC
LIBRARY   := libdjsp.a
OBJ_FLAGS :=
else
LIBRARY   := libdjsp.so
OBJ_FLAGS := -fPIC
endif



all: $(EXE)
	perl tester.pl

install: all
	@# mkdir -p /usr/local/include/djsp
	@# cp include/* /usr/local/include/djsp
	@# cp $(BUILD_DIR)/$(LIBRARY) /usr/local/lib
	@# cp djsp /usr/local/bin

	install -D -t /usr/local/include/djsp include/*
	install -D -t /usr/local/lib          $(BUILD_DIR)/$(LIBRARY)
	install -D -t /usr/local/bin          djsp

clean:
	-rm -rf $(BUILD_DIR) libdjsp.so libdjsp.a $(EXE)

.PHONY: all clean install



$(EXE): %: $(BUILD_DIR)/$(EXE_DIR)/%.o $(BUILD_DIR)/$(LIBRARY)
	$(CC) $(LDFLAGS) -o $@ $^ $(LOADLIBES) $(LDLIBS)

$(BUILD_DIR)/libdjsp.a: $(OBJ)
	ar rcs $@ $?
$(BUILD_DIR)/libdjsp.so: $(OBJ)
	$(CC) -shared -o $@ $^

$(BUILD_DIR)/$(OBJ_DIR)/%.o: $(OBJ_DIR)/%.c | $(BUILD_DIR)/$(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(OBJ_FLAGS) -c -o $@ $<
$(BUILD_DIR)/$(EXE_DIR)/%.o: $(EXE_DIR)/%.c | $(BUILD_DIR)/$(EXE_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/$(OBJ_DIR) $(BUILD_DIR)/$(EXE_DIR):
	mkdir -p $@
