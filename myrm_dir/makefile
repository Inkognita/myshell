all: target

target: myrm.o
	g++ -o myrm myrm.o -lboost_system -lboost_filesystem

myrm.o: myrm.cpp
	g++ -c myrm.cpp -lboost_system -lboost_filesystem

clean:
	rm myrm.o myrm



