TARGET = Delegate.exe

BIN_DIR 	= ./bin
BUILD_DIR = ./build
LIB_DIR 	= ./lib
SRC_DIR 	= ./src

CC = g++

CFLAGS = 	-std=c++17 	\
					-Wall 			\
					-Wextra			\
					-Wpedantic 	\

CFLAGS += -I $(SRC_DIR)

LDFLAGS =

ifeq ($(OS), Windows_NT)
	rwildcard = $(foreach d, $(wildcard $1*), $(call rwildcard,$d/,$2) \
											 $(filter $(subst *,%,$2), $d))

	SRCS = $(call rwildcard, $(SRC_DIR), *.cpp)
else
	SRCS = $(shell find $(SRC_DIR) -name "*.cpp")
endif

OBJS = $(SRCS:%=$(BUILD_DIR)/%.o)

.PHONY: default
default: dev

.PHONY: debug
debug: CFLAGS += -O0 -ggdb -D DEBUG
debug: target

.PHONY: dev
dev: CFLAGS += -O3 -ggdb -D DEBUG
dev: target

.PHONY: release
release: CFLAGS += -O3
release: target

.PHONY: target
target: $(OBJS)
	$(CC) $(OBJS) -o ./bin/$(TARGET) $(LDFLAGS)

./build/%.$.cpp.o: %.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	@rm -rf $(BIN_DIR)/$(TARGET)
	@rm -rf $(BUILD_DIR)/$(SRC_DIR) 