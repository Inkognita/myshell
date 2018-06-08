all: myshell mycp myrm mycat myls mymkdir mymv mygrep

myshell:
	g++ myshell.cpp -o myshell -std=c++11 -lreadline

mycp:
	g++-7 mycp_dir/mycp.cpp -o mycp -std=c++17 -lstdc++fs

myrm:
	g++ myrm_dir/myrm.cpp -o myrm -lboost_system -lboost_filesystem

mycat:
	g++ mycat_dir/main.c -o mycat

myls:
	g++-7 -std=c++14 myls_dir/myls.cpp -o myls -lboost_filesystem -lboost_system

mymkdir:
	g++ mymkdir_dir/mymkdir.cpp -o mymkdir -std=c++14

mymv:
	g++-7 mymv_dir/mymv.cpp -o mymv -std=c++17 -lstdc++fs

mygrep:
	make -f mygrep_dir/makefile

clean:
	rm myshell mycp myrm mycat myls mymkdir mymv mygrep
