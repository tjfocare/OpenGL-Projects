INCLUDE_PATHS = -I/usr/local/include -I/opt/X11/include
LIBRARY_PATHS = -L/usr/local/lib -I/opt/X11/lib
COMPILER_FLAGS = -Wall
LINKER_FLAGS = -framework OpenGL -lglfw -lglew

CC = g++
EXE = maze
OBJS = Driver.o Maze.o Sphere.o Shader.o Viewer.o

.PHONY: all clean

all: $(EXE)

$(EXE): $(OBJS)
	$(CC) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(EXE) $(OBJS)

Driver.o: Driver.cpp InputState.h
	$(CC) -c Driver.cpp

Shader.o : Shader.cpp Shader.hpp
	$(CC) -c Shader.cpp

Viewer.o: Viewer.h Viewer.cpp InputState.h
	$(CC) -c Viewer.cpp

Maze.o: Maze.h Maze.cpp
	$(CC) -c Maze.cpp

Sphere.o: Sphere.hpp Sphere.cpp
	$(CC) -c Sphere.cpp

clean:
	rm -f *.o $(EXE)
