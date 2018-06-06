all: target

target: myls.o
	g++-7 -std=c++14 -o myls -g myls.o -lboost_filesystem -lboost_system

myls.o: myls.cpp
	g++-7 -std=c++14 -c myls.cpp -g -lboost_filesystem -lboost_system

clean:
	rm myls.o myls

