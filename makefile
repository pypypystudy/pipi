# obj: main.o
# Create by pipi

CPL  = D:/cygwin/bin/
BIN  = pipi
CPP  = $(CPL)g++.exe
RM = rm -f

LINKOBJ  = obj/main.o \
					 obj/base64.o \
					 obj/md5.o \
					 obj/aes.o

all: main.o base64.o md5.o aes.o
	$(CPP) $(LINKOBJ) -o $(BIN)
	
clean:
	${RM} obj/*.o
	${RM} *.exe

include base64/makefile
include main/makefile
include md5/makefile
include aes/makefile
