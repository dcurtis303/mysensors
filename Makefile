CXX = g++
CXXFLAGS = -g -Wall -Wextra -Wpedantic -Wno-deprecated
LIBS = -lpthread -lsensors -lncurses -lconfig++

BIN = mysensors
BUILDDIR = ./build

SOURCES = $(wildcard src/*.cpp)
OBJECTS = $(SOURCES:%.cpp=$(BUILDDIR)/%.o)
DEPENDS = $(OBJECTS:%.o=%.d)

all: $(BIN)

$(BIN): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

-include $(DEPENDS)

$(BUILDDIR)/%.o : %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

clean:
	-rm -f $(BIN)
	-rm -rf $(BUILDDIR)
