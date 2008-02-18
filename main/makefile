# obj: main.o
# Create by pipi

CPL  = d:/Dev-Cpp/bin/
CPP  = $(CPL)g++.exe
ACXXFLAGS =  -I"d:/Dev-Cpp/lib/gcc/mingw32/3.4.2/include"  \
            -I"d:/Dev-Cpp/include/c++/3.4.2/backward"  \
            -I"d:/Dev-Cpp/include/c++/3.4.2/mingw32"  \
            -I"d:/Dev-Cpp/include/c++/3.4.2"  \
            -I"d:/Dev-Cpp/include" 

main.o: main/main.cpp main/pipi.h
	$(CPP) -c main/main.cpp -o obj/main.o $(ACXXFLAGS)
