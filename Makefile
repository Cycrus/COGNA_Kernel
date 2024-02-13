#-----------------------------------------------------------------------------------------------------------------------
# Compiler Definitions
#

CC = gcc -Wall -std=c14
CXX = g++ -Wall -std=c++14 -pthread

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

.PHONY: test_utilities
test_utilities:
	@echo "########### Testing utilities. ###########" ;
	@echo "Testing logger." ; \
	./build/tests/logger_test ;
	@echo "Testing pointer." ; \
	./build/tests/pointer_test ;

.PHONY: test_udp_sockets
test_udp_sockets:
	@echo ""########### Testing udp sockets. ###########"
	@echo "Starting UDP receiver." ; \
	./build/tests/udp_receiver_test & \
	prog_pid_receiver=$$! ; \
	sleep 1 ; \
	echo "Starting UDP sender." ; \
	./build/tests/udp_sender_test ; \
	sleep 2 ; \
	echo "Stopping UDP receiver" ; \
	kill $$prog_pid_receiver ;

.PHONY: test_network_builder
test_network_builder:
	@echo ""########### Testing network builder. ###########" ;
	@echo "Starting network." ; \
	cd build/tests ; \
	./builder_test Variants_Test & \
	prog_pid=$$! ; \
	sleep 3 ; \
	echo "Stopping network." ; \
	kill $$prog_pid ;

.PHONY: test_reinforcement_learning
test_reinforcement_learning:
	@echo ""########### Testing simple reinforcement learning. ###########"
	@./build/tests/simple_reinforcement_learning_test
	@echo "Test successful."

.PHONY: test_aplysia
test_aplysia:
	@echo ""########### Testing Aplysia. ###########" ;
	@echo "Starting aplysia network." ; \
	./build/tests/aplysia_test & \
	prog_pid=$$! ; \
	sleep 1 ; \
	echo "Stopping aplysia network." ; \
	kill $$prog_pid ;

clean:
	rm -rf build
