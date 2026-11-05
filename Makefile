CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

all: Kea

Kea: main.cpp
	$(CXX) $(CXXFLAGS) main.cpp -o Kea $$(pkg-config --cflags --libs Qt6Widgets)

clean:
	rm -f WebGuard

.PHONY: all clean