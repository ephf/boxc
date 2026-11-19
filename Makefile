CXXFLAGS = -std=c++23 -Wall -Wextra -Iinclude/
DBGFLAGS = -g -ggdb -O0
SRCS = src/main.cpp $(wildcard src/*/*.cpp)
TARGET = boxc

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

debug: $(SRCS)
	$(CXX) $(CXXFLAGS) $(DBGFLAGS) $(SRCS) -o $(TARGET)

all: $(TARGET)

clean:
	$(RM) $(TARGET)