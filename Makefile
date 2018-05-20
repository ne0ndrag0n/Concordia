CC = g++
CFLAGS = -pthread -std=c++17 -g -rdynamic -pipe #-fsanitize=address
DFLAGS = -DSOL_SAFE_FUNCTION=1
INCLUDES = -Iinclude -Ilib -Ilib/nanovg
LIBPATHS = -Llib/nanovg
LIBS = -lpthread -ltbb -lassimp -lGLEW -lGL -lsfml-graphics -lsfml-window -lsfml-system -ltinyxml2 -ljsoncpp -llua -ldl -lnanovg

SRCS = $(wildcard src/*.cpp)
SRCS += $(wildcard src/device/*.cpp)
SRCS += $(wildcard src/device/display/*.cpp)
SRCS += $(wildcard src/device/display/adapter/*.cpp)
SRCS += $(wildcard src/device/display/adapter/component/*.cpp)
SRCS += $(wildcard src/device/input/*.cpp)
SRCS += $(wildcard src/graphics/*.cpp)
SRCS += $(wildcard src/graphics/imagebuilder/*.cpp)
SRCS += $(wildcard src/graphics/scenegraph/*.cpp)
SRCS += $(wildcard src/graphics/scenegraph/animation/*.cpp)
SRCS += $(wildcard src/graphics/scenegraph/mesh/*.cpp)
SRCS += $(wildcard src/graphics/scenegraph/modelloader/*.cpp)
SRCS += $(wildcard src/graphics/userinterface/*.cpp)
SRCS += $(wildcard src/graphics/userinterface/event/*.cpp)
SRCS += $(wildcard src/graphics/userinterface/style/*.cpp)
SRCS += $(wildcard src/graphics/userinterface/widgets/*.cpp)
SRCS += $(wildcard src/graphics/vector/*.cpp)
SRCS += $(wildcard src/models/*.cpp)
SRCS += $(wildcard src/scripting/*.cpp)
SRCS += $(wildcard src/scripting/entitykit/*.cpp)
SRCS += $(wildcard src/scripting/entitykit/components/*.cpp)
SRCS += $(wildcard src/scripting/event/*.cpp)
SRCS += $(wildcard src/scripting/luakit/*.cpp)
SRCS += $(wildcard src/state/*.cpp)
SRCS += $(wildcard src/threading/*.cpp)
SRCS += $(wildcard src/tools/*.cpp)

OBJS = $(SRCS:.cpp=.o)

MAIN = bbexec

.PHONY: clean

all:    $(MAIN)
		@echo  BlueBear built successfully.

$(MAIN): $(OBJS)
		$(CC) $(CFLAGS) $(INCLUDES) $(LIBPATHS) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)
.cpp.o:
		$(CC) $(CFLAGS) $(DFLAGS) $(INCLUDES) -c $<  -o $@

clean:
		$(RM) *.o *~ $(MAIN)
		find src/ -name "*.o" -type f -delete

run:    ${MAIN}
	./bbexec
