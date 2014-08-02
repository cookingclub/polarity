set CXX=em++
set LDFLAGS=-s STB_IMAGE=1 --preload-file assets
set OBJS=
set SRCS=graphics\*.cpp main\*.cpp

for %%i in (%SRCS%) DO set OBJS= %OBJS% %%~dpni.o


set OBJS=graphics/sprite.o main/main.o main/loop.o
%CXX% %OBJS% %LDFLAGS% -o %EXE%