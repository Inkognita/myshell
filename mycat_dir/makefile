all: target

target: main.o
	g++ -o mycat main.o
main.o: main.c
	g++ -c $< -o $@
clean:
	rm main.o mycat
