# obj: main.o
# Create by pipi

CPL  = D:/cygwin/bin/
CPP  = $(CPL)g++.exe
ACXXFLAGS =  -I"D:/cygwin/lib/gcc/i686-pc-cygwin/3.4.4"\
            -I"D:/cygwin/usr/include" 

all: kaes.o
	$(CPP) kaes.o -o kaes

kaes.o: kaes.cpp kaes.h
	$(CPP) -c kaes.cpp -o kaes.o
