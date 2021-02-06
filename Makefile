CC = g++ -Wall -std=c++11

INCLUDES = -I inc/

SOURCES = $(wildcard src/*.cpp)
HEADERS = $(wildcard inc/*.h)
OBJECTS = $(subst src, build/objects, $(SOURCES:.cpp=.o))

SRC_TEST = $(wildcard src_test/*.cpp)
TARGET_TEST = $(subst src_test, build/tests, $(SRC_TEST:.cpp=))

tests: $(TARGET_TEST)
	@echo "Tests done."

build/tests/%: src_test/%.cpp $(OBJECTS)
	$(CC) $< $(OBJECTS) -o $@ $(INCLUDES) -lm

build/objects/%.o: src/%.cpp build
	$(CC) -c $< -o $@ $(INCLUDES)

build:
	mkdir -p build/objects
	mkdir -p build/tests

clean:
	rm -rf $(TARGET) $(OBJECTS)