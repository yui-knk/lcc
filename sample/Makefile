BUILD_DIR=build

.PHONY: all build clean

all: build

build:
	mkdir -p $(BUILD_DIR)
	gcc -o $(BUILD_DIR)/test1 test1.c
	gcc -o $(BUILD_DIR)/test2 test2.s
	gcc -o $(BUILD_DIR)/plus1 plus1.c
	gcc -o $(BUILD_DIR)/plus2 plus2.s
	gcc -o $(BUILD_DIR)/step2_test1 step2_test1.s

clean:
	rm -rf $(BUILD_DIR)
