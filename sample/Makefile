BUILD_DIR=build

.PHONY: all build clean

all: build

build:
	mkdir -p $(BUILD_DIR)
	gcc -o $(BUILD_DIR)/test1 test1.c
	gcc -o $(BUILD_DIR)/test2 test2.s

clean:
	rm -rf $(BUILD_DIR)
