set EXE=polarity.html
set OBJS=graphics/*.o main/*.o

set CURDIR=%CD%

rem tinyxml2/tinyxml2.o libtmx-parser/src/tmxparser.o

set CC=emcc
set CXX=em++

set CFLAGS=-pthread -g -O2 -I %CURDIR%
rem -Ilibtmx-parser/src -Itinyxml2
set CXXFLAGS=-std=gnu++11 $(CFLAGS)
set LDFLAGS=-s STB_IMAGE=1 --preload-file assets

for %%i in %SRCS% (
    %CXX% %CXXFLAGS% %%i -c -o %%~dpni.o
)

%CXX% %OBJS% %LDFLAGS% -o %EXE%

