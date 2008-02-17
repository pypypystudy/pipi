cd base64
d:\Dev-Cpp\bin\make.exe all
move base64.o ..\obj

cd..
cd main
d:\Dev-Cpp\bin\make.exe all
move main.o ..\obj

cd..
cd obj
d:\Dev-Cpp\bin\make.exe all
move pipi.exe ..\
cd..