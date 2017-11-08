CC = gcc
override CFLAGS += -Wall -Wextra -std=c11
CPPC = g++
CPPFLAGS = -Wall -Wextra -std=c++11

BUILD_DIR           = build
TEST_DIR            = test
INSTALL_LIB_DIR     = /usr/local/lib
INSTALL_INCLUDE_DIR = /usr/local/include

DARRAY_HEADER  = darray.h
DARRAY_OBJ     = darray.o
DARRAY_LIB     = darray
DARRAY_LIB_OUT = lib$(DARRAY_LIB).a

default: release

debug: CFLAGS += -g -DDEBUG
debug: build unit_tests

release: CFLAGS += -Werror -O3 -DNDEBUG
release: build unit_tests

build: clean
	mkdir -p $(BUILD_DIR)
	gcc $(CFLAGS) -c -o $(BUILD_DIR)/$(DARRAY_OBJ) darray.c $(OPTIMIZATION_LEVEL)
	ar rcs $(BUILD_DIR)/$(DARRAY_LIB_OUT) $(BUILD_DIR)/$(DARRAY_OBJ)

install: build
	install $(BUILD_DIR)/$(DARRAY_LIB_OUT) $(INSTALL_LIB_DIR)/$(DARRAY_LIB_OUT)
	install $(DARRAY_HEADER) $(INSTALL_INCLUDE_DIR)/$(DARRAY_HEADER)

# Requires the environment variable EMU_ROOT to be set to the root directory of
# Extended MinUnit (https://github.com/VictorSCushman/EMU).
unit_tests: build
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/unit_tests $(TEST_DIR)/darray.test.c -I${EMU_ROOT} -L$(BUILD_DIR) -l$(DARRAY_LIB)
	cp -r $(TEST_DIR)/resources $(BUILD_DIR)
	cd $(BUILD_DIR); ./unit_tests

perf_tests: build
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/perf_tests_darr $(TEST_DIR)/perf_tests/perf.test.c -L$(BUILD_DIR) -l$(DARRAY_LIB)
	$(CPPC) $(CPPFLAGS) -o $(BUILD_DIR)/perf_tests_vector $(TEST_DIR)/perf_tests/perf.test.cpp

clean:
	@rm -rf $(BUILD_DIR)
