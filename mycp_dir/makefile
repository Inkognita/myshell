all: target

target: mycp.o
	g++-7 -o mycp mycp.o -std=c++17 -lstdc++fs

mycp.o: mycp.cpp
	g++-7 -c mycp.cpp -std=c++17 -lstdc++fs

clean:
	rm mycp.o mycp
