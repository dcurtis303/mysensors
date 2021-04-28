CXX = g++
CXXFLAGS = -g -Wall -Wextra -Wpedantic -Wno-deprecated
LIBS = -lpthread -lsensors -lncurses -lconfig++

BIN = mysensors

SOURCES = $(wildcard src/*.cpp)
OBJECTS = $(SOURCES:%.cpp=%.o)
DEPENDS = $(OBJECTS:%.o=%.d)

all: $(BIN)

$(BIN): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

-include $(DEPENDS)

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

clean:
	-rm -f $(OBJECTS) $(DEPENDS) $(BIN)
