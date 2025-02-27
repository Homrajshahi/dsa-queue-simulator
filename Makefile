CC = gcc
CFLAGS = -Wall -Iinclude -I/usr/include/SDL2
LDFLAGS = -L/usr/lib/x86_64-linux-gnu -lSDL2 -lSDL2_ttf -lm

all: simulator

simulator: src/simulator.o src/queue.o src/traffic_generator.o
	$(CC) $(CFLAGS) -o bin/simulator src/simulator.o src/queue.o src/traffic_generator.o $(LDFLAGS)

src/simulator.o: src/simulator.c include/queue.h include/traffic_generator.h
	$(CC) $(CFLAGS) -c src/simulator.c -o src/simulator.o

src/traffic_generator.o: src/traffic_generator.c include/traffic_generator.h
	$(CC) $(CFLAGS) -c src/traffic_generator.c -o src/traffic_generator.o

src/queue.o: src/queue.c include/queue.h
	$(CC) $(CFLAGS) -c src/queue.c -o src/queue.o

clean:
	rm -f src/*.o bin/simulator

