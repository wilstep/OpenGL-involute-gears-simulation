gear: gear.o matrix.o main.o
	g++ -o gear main.o matrix.o gear.o -lglfw -lGLEW -lGL 

gear.o: gear.cpp gear.h
	g++ -std=c++17 -c gear.cpp

matrix.o: matrix.cpp matrix.h
	g++ -std=c++17 -c matrix.cpp

main.o: main.cpp matrix.h gear.h 
	g++ -std=c++17 -c main.cpp

