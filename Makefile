#-----------------------------------------------------------------------------------------------------------------------
# Compiler Definitions
#

CC = gcc -Wall -std=c14
CXX = g++ -Wall -std=c++14

#-----------------------------------------------------------------------------------------------------------------------
# Compiler Flags
#

INCLUDES = -I inc/ -I src/header_only_libs

CFLAGS = $(INCLUDES)
LDFLAGS = -lm

#-----------------------------------------------------------------------------------------------------------------------
# Files
#

HEADERS = $(wildcard inc/*.h) $(wildcard inc/*.hpp)

SRCDIRS = $(sort $(dir $(wildcard src/*/)))
SRC_C = $(foreach dir,$(SRCDIRS),$(wildcard $(dir)*.c))
SRC_CPP = $(foreach dir,$(SRCDIRS),$(wildcard $(dir)*.cpp))
HEADER_ONLY_HPP = $(foreach dir,$(SRCDIRS),$(wildcard $(dir)*.hpp))

OBJ_C = $(subst src, build/objects, $(SRC_C:.c=.o))
OBJ_CPP = $(subst src, build/objects, $(SRC_CPP:.cpp=.o))
OBJ_HEADER_ONLY = $(subst src, build/objects, $(HEADER_ONLY_HPP:.hpp=.o))
OBJECTS = $(OBJ_HEADER_ONLY) $(OBJ_C) $(OBJ_CPP)

OBJDIRS = $(sort $(dir $(OBJ_HEADER_ONLY))) $(sort $(dir $(OBJ_C))) $(sort $(dir $(OBJ_CPP)))

SRC_TEST = $(wildcard src_test/*.cpp)
TARGET_TEST = $(subst src_test, build/tests, $(SRC_TEST:.cpp=))

#-----------------------------------------------------------------------------------------------------------------------
# Build Rules
#


tests: $(TARGET_TEST)
	@echo "Tests done."

build/tests/%: src_test/%.cpp $(OBJECTS)
	$(CXX) $< $(OBJECTS) -o $@ $(CFLAGS) $(LDFLAGS)

build/objects/%.o: src/%.c build
	$(CC) -c $< -o $@ $(CFLAGS)

build/objects/%.o: src/%.cpp build
	$(CXX) -c $< -o $@ $(CFLAGS)

build/objects/%.o: src/%.hpp build
	$(CXX) -c -x c++ $< -o $@

build:
	mkdir -p build/objects
	mkdir -p build/tests
	for dir in $(OBJDIRS); do mkdir -p $$dir; done

clean:
	rm -rf build
