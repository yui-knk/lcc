SRC_DIR=src
BUILD_DIR=build

.PHONY: all build clean test

all: build

build:
	mkdir -p $(BUILD_DIR)
	gcc -Wall -o $(BUILD_DIR)/lcc $(SRC_DIR)/main.c

clean:
	rm -rf $(BUILD_DIR)

test: build
	BUILD_DIR=$(BUILD_DIR) ./test.sh
