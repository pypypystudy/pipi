# obj: main.o
# Create by pipi

############START modified to be correct information##############
#CPL: address of make.exe                                        #
#OBJ: object to be compiled                                      #
#SRC: source to be compiled                                      #
##################################################################
CPL  = d:/Dev-Cpp/bin/
OBJ  = main.o
SRC  = main.cpp
#############END modified to be correct information###############


CPP  = $(CPL)g++.exe
CXXFLAGS =  -I"d:/Dev-Cpp/lib/gcc/mingw32/3.4.2/include"  \
            -I"d:/Dev-Cpp/include/c++/3.4.2/backward"  \
            -I"d:/Dev-Cpp/include/c++/3.4.2/mingw32"  \
            -I"d:/Dev-Cpp/include/c++/3.4.2"  \
            -I"d:/Dev-Cpp/include" 
RM = rm -f

all: 
	${RM} $(OBJ) $(BIN)
	$(CPP) -c $(SRC) -o $(OBJ) $(CXXFLAGS)