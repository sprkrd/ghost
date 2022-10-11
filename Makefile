ghost: ghost.cpp
	g++ -O3 -Wall -Werror -pedantic -std=c++17 -o ghost ghost.cpp

clean:
	rm -rf ghost
