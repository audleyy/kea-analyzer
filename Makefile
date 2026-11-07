CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

all: curl_test

curl_test: test.cpp
	$(CXX) $(CXXFLAGS) test.cpp -o curl_test $$(pkg-config --cflags --libs libcurl)

clean:
	rm -f curl_test

.PHONY: all clean