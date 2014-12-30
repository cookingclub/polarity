INCLUDES =  $(wildcard graphics/*.hpp audio/*.hpp physics/*.hpp main/*.hpp world/*.hpp)
ARSRCS = $(wildcard libs/box2d/Box2D/Box2D/Collision/Shapes/*.cpp libs/box2d/Box2D/Box2D/Dynamics/*.cpp libs/box2d/Box2D/Box2D/Common/*.cpp  libs/box2d/Box2D/Box2D/Rope/*.cpp libs/box2d/Box2D/Box2D/Dynamics/Contacts/*.cpp libs/box2d/Box2D/Box2D/Dynamics/Joints/*.cpp libs/box2d/Box2D/Box2D/Collision/*.cpp libs/libpng/*.c libs/zlib/[^g]*.c)

SRCS = $(wildcard graphics/*.cpp physics/*.cpp main/*.cpp world/*.cpp libs/libtmxparser/src/*.cpp )
# audio/*.cpp

O_EXT = .o
AR_EXT = .a

OBJ_NATIVE = .objs/native
EXE_NATIVE = polarity
OBJ_EM = .objs/em
EXE_EM = polarity.html

ifdef NATIVE
OBJ_DIR = $(OBJ_NATIVE)
EXE = $(EXE_NATIVE)

CC = gcc
CXX = g++
AR = ar
  ifdef SDL1
LDFLAGS = -lSDL -lSDL_mixer -lSDLmain $(EXTERNALS) -lGL -lSDL_gfx -lSDL_ttf
PLATFORM_CFLAGS =
  else
LDFLAGS = -Llibs/libsdl/lib  $(EXTERNALS)  -lSDL2_mixer -lGL -lSDL2_ttf -lSDL2 -lSDL2main -lrt -lfreetype
PLATFORM_CFLAGS = -Ilibs/libsdl/include -DUSE_SDL2
  endif
else
OBJ_DIR = $(OBJ_EM)
EXE = $(EXE_EM)

CC = emcc
CXX = em++
AR = emar
LDFLAGS = -s EXPORTED_FUNCTIONS="['_main','_onContextLost','_onContextRestored']" -s TOTAL_MEMORY=134217728 $(EXTERNALS)
PLATFORM_CFLAGS = -DUSE_GLES
# --preload-file assets
endif

EXTERNALS = $(OBJ_DIR)/libext$(AR_EXT)

AROBJS = $(patsubst %.c, $(OBJ_DIR)/%$(O_EXT), $(patsubst %.cpp, $(OBJ_DIR)/%$(O_EXT), $(ARSRCS))) $(OBJ_DIR)/libs/tinyxml2/tinyxml2$(O_EXT)
OBJS = $(patsubst %.cpp, $(OBJ_DIR)/%$(O_EXT), $(SRCS))

CFLAGS = $(PLATFORM_CFLAGS) -fno-exceptions -pthread -g -Wall -Wextra -Wno-unused-parameter -Wno-warn-absolute-paths -I $(CURDIR) -I$(CURDIR)/libs/box2d/Box2D/ -I$(CURDIR)/libs/tinyxml2 -I$(CURDIR)/libs/tinyxml2 -I$(CURDIR)/libs/libtmxparser/src -I$(CURDIR)/libs/zlib -I$(CURDIR)/libs/libpng -I$(CURDIR)/libs/libsdl/include
CXXFLAGS = -std=gnu++11 $(CFLAGS)

$(OBJ_DIR)/%$(O_EXT): %.cpp
	mkdir -p `dirname $@`
	$(CXX) $(CXXFLAGS) $< -c -o $@

$(OBJ_DIR)/%$(O_EXT): %.c
	mkdir -p `dirname $@`
	$(CC) $(CFLAGS) $< -c -o $@

$(EXE): $(OBJS) $(wildcard assets/*) $(EXTERNALS)
	$(CXX) $(OBJS) $(LDFLAGS) -o $@

$(EXTERNALS): $(AROBJS)
	$(AR) cru $@ $(AROBJS)

$(OBJS): $(INCLUDES) Makefile

clean:
	rm -f $(patsubst %.c, %.*o, $(patsubst %.cpp, %.*o, $(ARSRCS))) libs/tinyxml2/tinyxml2.*o
	rm -f $(patsubst %.cpp, %.*o, $(SRCS))
	rm -f libs/libext.*a graphics/sprite.o libext.a physics/behavior.o physics/physics.o
	rm -f $(OBJS) $(AROBJS) $(EXTERNALS)
	rm -f $(EXE)

