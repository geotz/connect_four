CXXFLAGS=-O2 -Wall -std=c++14

#LDFLAGS=-framework sfml-window -framework sfml-graphics -framework sfml-system
LDLIBS=-lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system
#LDFLAGS=-L/usr/lib/x86_64-linux-gnu

all: connect_four

connect_four: connect_four.cpp framerateadjuster.o game.o gameview.o audio.o

clean:
	rm -f connect_four *.o
