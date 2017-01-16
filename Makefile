CC = g++
CFLAGS = -pthread -std=c++14 -g -rdynamic -pipe
INCLUDES = -Iinclude
LIBS = -lpthread -ltbb -lassimp -lGLEW -lGL -lsfml-graphics -lsfml-window -lsfml-system -lsfgui -ltinyxml2 -ljsoncpp -llua -ldl

SRCS = $(wildcard src/*.cpp)
SRCS += $(wildcard src/graphics/*.cpp)
SRCS += $(wildcard src/graphics/gui/*.cpp)
SRCS += $(wildcard src/graphics/imagebuilder/*.cpp)
SRCS += $(wildcard src/graphics/instance/*.cpp)
SRCS += $(wildcard src/scripting/*.cpp)
SRCS += $(wildcard src/scripting/event/*.cpp)
SRCS += $(wildcard src/scripting/luakit/*.cpp)
SRCS += $(wildcard src/threading/*.cpp)
SRCS += $(wildcard src/tools/*.cpp)
OBJS = $(SRCS:.cpp=.o)

MAIN = bbexec

.PHONY: clean

all:    $(MAIN)
		@echo  BlueBear built successfully.

$(MAIN): $(OBJS)
		$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)
.cpp.o:
		$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
		$(RM) *.o *~ $(MAIN)
		find src/ -name "*.o" -type f -delete

run:    ${MAIN}
	./bbexec
