FLAGS=-std=c++17

main: main.o
	g++ $(FLAGS) main.o -o main

main.o:	main.cpp
	g++ $(FLAGS) -c main.cpp