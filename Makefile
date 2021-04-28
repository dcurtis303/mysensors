CXX = g++
CXXFLAGS = -g -Wall -Wextra -Wpedantic -Wno-deprecated
LDFLAGS =
LIBS = -lpthread -lsensors -lncurses -lconfig++

BIN = mysensors
BUILD_DIR = ./build

SOURCES = $(wildcard src/*.cpp)
OBJECTS = $(SOURCES:%.cpp=$(BUILD_DIR)/%.o)
DEPENDS = $(OBJECTS:%.o=%.d)

all: $(BIN)

$(BIN): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

-include $(DEPENDS)
$(BUILD_DIR)%.o : %.cpp
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

clean:
	-rm -f $(OBJECTS) $(DEPENDS) mysensors
