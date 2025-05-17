# Makefile
CXX       = g++
CXXFLAGS  = -std=c++17 -O2 -Wall
TARGET    = mt19937_clone
SRC       = mt19937_clone.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)
