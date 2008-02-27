CPL  = D:/cygwin/bin/
CPP  = $(CPL)g++.exe
ACXXFLAGS =  -I"D:/cygwin/lib/gcc/i686-pc-cygwin/3.4.4"\
            -I"D:/cygwin/usr/include" 

decodepacket: main.o md5.o aes.o
	$(CPP) main.o md5.o aes.o -o decodepacket

decodepacket.o: main.cpp decodepacket.h
	$(CPP) -c main.cpp -o main.o
