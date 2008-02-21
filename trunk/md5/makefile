# obj: main.o
# Create by pipi

CPL  = D:/cygwin/bin/
CPP  = $(CPL)g++.exe
ACXXFLAGS =  -I"D:/cygwin/lib/gcc/i686-pc-cygwin/3.4.4"\
            -I"D:/cygwin/usr/include" 

all: md5.o
	$(CPP) md5.o -o md5

md5.o: md5.cpp md5.h
	$(CPP) -c md5.cpp -o md5.o
