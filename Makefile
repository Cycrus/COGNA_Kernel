#-----------------------------------------------------------------------------------------------------------------------
# Compiler Definitions
#

CC = gcc -Wall -std=c11
CXX = g++ -Wall -std=c++11

#-----------------------------------------------------------------------------------------------------------------------
# Compiler Flags
#

INCLUDES = -I inc/

CFLAGS = $(INCLUDES)
LDFLAGS = -lm

#-----------------------------------------------------------------------------------------------------------------------
# Files
#

HEADERS = $(wildcard inc/*.h) $(wildcard inc/*.hpp)

SRCDIRS = $(sort $(dir $(wildcard src/*/)))
SRC_C = $(foreach dir,$(SRCDIRS),$(wildcard $(dir)*.c))
SRC_CPP = $(foreach dir,$(SRCDIRS),$(wildcard $(dir)*.cpp))

OBJ_C = $(subst src, build/objects, $(SRC_C:.c=.o))
OBJ_CPP = $(subst src, build/objects, $(SRC_CPP:.cpp=.o))
OBJECTS = $(OBJ_C) $(OBJ_CPP)

OBJDIRS = $(sort $(dir $(OBJ_C))) $(sort $(dir $(OBJ_CPP)))

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

build:
	mkdir -p build/objects
	mkdir -p build/tests
	for dir in $(OBJDIRS); do mkdir -p $$dir; done

clean:
	rm -rf $(TARGET_TEST) $(OBJECTS)