all: target

target: mymkdir.o
	g++ -o mymkdir mymkdir.o -std=c++14

mymkdir.o: mymkdir.cpp
	g++ -c mymkdir.cpp -std=c++14

clean:
	rm mymkdir.o mymkdir
