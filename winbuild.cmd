set EXE=polarity.html
set SRCS=graphics\*.cpp main\*.cpp

set CURDIR=%CD%

rem tinyxml2/tinyxml2.o libtmx-parser/src/tmxparser.o

set CC=emcc
set CXX=em++

set CFLAGS=-pthread -g -O2 -I %CURDIR%
rem -Ilibtmx-parser/src -Itinyxml2
set CXXFLAGS=-std=gnu++11 %CFLAGS%
set LDFLAGS=-s STB_IMAGE=1 --preload-file assets
echo %SRCS%
set OBJS=
for %%i in (%SRCS%) DO %CXX% %CXXFLAGS% %%~dpni.cpp -c -o %%~dpni.o
   set OBJS= %OBJS% %%~dpni.o
echo hi
%CXX% %OBJS% %LDFLAGS% -o %EXE%

