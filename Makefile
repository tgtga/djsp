  OBJ_DIR := src
BUILD_DIR := build

OBJ_SRC := $(wildcard $(OBJ_DIR)/*.c)
OBJ     := $(OBJ_SRC:%.c=$(BUILD_DIR)/%.o)

CC := clang

# for the future: add 'override' at the beginning
# if you want to append to a variable set at invocation
CFLAGS := \
	-std=c99 \
  -O3 -march=native -mtune=native \
  -Wall -Wextra -Wpedantic \
	-g
CFLAGS += $(WHATEVER)
LDFLAGS := -lgmp

# ifdef STATIC
# LIBRARY   := libdjsp.a
# OBJ_FLAGS :=
# else
LIBRARY   := libdjsp.so
OBJ_FLAGS := -fPIC
# endif



all: $(BUILD_DIR)/libdjsp.so
	echo $(CFLAGS)
	./tester.rb

install: all
	install -D -t /usr/local/include/djsp include/*
	install -D -t /usr/local/lib          $(BUILD_DIR)/$(LIBRARY)
	install -D -t /usr/local/bin          djsp

clean:
	-rm -rf $(BUILD_DIR) libdjsp.so libdjsp.a

.PHONY: all clean install



# $(BUILD_DIR)/libdjsp.a: $(OBJ)
# 	ar rcs $@ $?
$(BUILD_DIR)/libdjsp.so: $(OBJ)
	$(CC) -shared -lgmp -o $@ $^

$(BUILD_DIR)/$(OBJ_DIR)/%.o: $(OBJ_DIR)/%.c | $(BUILD_DIR)/$(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(OBJ_FLAGS) -c -o $@ $<
$(BUILD_DIR)/$(EXE_DIR)/%.o: $(EXE_DIR)/%.c | $(BUILD_DIR)/$(EXE_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/$(OBJ_DIR) $(BUILD_DIR)/$(EXE_DIR):
	mkdir -p $@
