# Compiles the sttyl program
#
# Default "make" command should make sttyl from the sttyl.o and termops.o
# files.
#
# sttyl.o has a dependancy on termops.h, and terops has a dependancy on
# termops.h and util.h
#
# All files are compiled with warnings and debugging flags.
CC = g++
CFLAGS = -Wall -c -g
LFLAGS = -Wall -g

invaders : invaders.o timer.o board.o engine.o sprite.o animation.o
	$(CC) $(LFLAGS) invaders.o board.o timer.o engine.o sprite.o animation.o -o invaders -lSDL2 -lSDL2_image

invaders.o : invaders.cpp invaders.h board.h engine.h
	$(CC) $(CFLAGS) invaders.cpp

timer.o : timer.cpp timer.h
	$(CC) $(CFLAGS) timer.cpp

board.o : board.cpp board.h sprite.h animation.h
	$(CC) $(CFLAGS) board.cpp

engine.o : engine.cpp engine.h timer.h board.h
	$(CC) $(CFLAGS) engine.cpp

sprite.o : sprite.cpp sprite.h board.h animation.h
	$(CC) $(CFLAGS) sprite.cpp

animation.o : animation.cpp animation.h
	$(CC) $(CFLAGS) animation.cpp

clean: 
	rm -rf *.o invaders 
