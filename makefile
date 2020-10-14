all: shell table builtIns main

main: seashell.o processTable.o builtIns.o processTable.h builtIns.h seashell.h
	g++ -std=c++11 -Wall -W -o seashell seashell.o processTable.o builtIns.o

shell: seashell.cpp seashell.h
	g++ -std=c++11 -Wall -W -c seashell.cpp

table: processTable.cpp processTable.h
	g++ -std=c++11 -Wall -W -c processTable.cpp

builtIns: builtIns.cpp builtIns.h
	g++ -std=c++11 -Wall -W -c builtIns.cpp
