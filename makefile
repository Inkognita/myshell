all: target

target: myshell.o
	g++ -o myshell myshell.o -std=c++11 -lreadline

myshell.o: myshell.cpp
	g++ -c myshell.cpp -std=c++11 -lreadline

clean:
	rm myshell.o myshell
