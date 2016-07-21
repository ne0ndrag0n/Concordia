CC = g++
CFLAGS = -std=c++14 -g -rdynamic
INCLUDES = -Iinclude
LIBS = -lpthread -lassimp -lGLEW -lGL -lsfml-graphics -lsfml-window -lsfml-system -ljsoncpp -llua -ldl

SRCS = $(wildcard src/*.cpp)
SRCS += $(wildcard src/graphics/*.cpp)
SRCS += $(wildcard src/scripting/*.cpp)
SRCS += $(wildcard src/threading/*.cpp)
OBJS = $(SRCS:.c=.o)

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
