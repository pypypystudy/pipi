# obj: main.o
# Create by pipi

CPL  = d:/Dev-Cpp/bin/
BIN  = pipi.exe
CPP  = $(CPL)g++.exe
RM = rm -f

LINKOBJ  = obj/main.o \
					 obj/base64.o 

all: main.o base64.o
	$(CPP) $(LINKOBJ) -o $(BIN)
	
clean:
	${RM} obj/*.o
	${RM} *.exe

include base64/makefile
include main/makefile
