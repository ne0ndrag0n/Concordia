CC = g++
CFLAGS = -std=c++14 -g -rdynamic
INCLUDES = -Iinclude/json -Iinclude/json/json -Iinclude/bluebear
LIBS = -llua -ldl

SRCS = $(wildcard src/*.cpp)
SRCS += include/json/jsoncpp.cpp
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
