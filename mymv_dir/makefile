all: target

target: mymv.o
	g++-7 -o mymv mymv.o -std=c++17 -lstdc++fs

mymv.o: mymv.cpp
	g++-7 -c mymv.cpp -std=c++17 -lstdc++fs

clean:
	rm mymv.o mymv
