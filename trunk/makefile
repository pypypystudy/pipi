# obj: main.o
# Create by pipi

############START modified to be correct information##############
#CPL:     address of make.exe                                    #
#BIN:     destnation file to be linked                           #
#LINKOBJ: objects to be linked                                   #
##################################################################
CPL  = d:/Dev-Cpp/bin/
BIN  = pipi.exe
LINKOBJ  = main.o \
					 base64.o 
#############END modified to be correct information###############


CPP  = $(CPL)g++.exe
RM = rm -f

all: 
	$(CPP) $(LINKOBJ) -o $(BIN)
	
clean:
	${RM} *.o