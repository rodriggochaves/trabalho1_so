# compilador
CC=clang++

# bibliotecas
LIBS = -std=c++11 -Wall

all: clean semaphore at scheduler execution_manager hello_world shutdown

scheduler:
	$(CC) $(LIBS) -c scheduler.cpp
	$(CC) $(LIBS) -o scheduler semaphore.o scheduler.o

execution_manager:
	$(CC) $(LIBS) -c execution_manager.cpp
	$(CC) $(LIBS) -o execution_manager semaphore.o execution_manager.o

at:
	$(CC) $(LIBS) -o at at.cpp

hello_world:
	$(CC) $(LIBS) -o hello_world hello_world.cpp

shutdown:
	$(CC) $(LIBS) -o shutdown shutdown.cpp

semaphore:
	$(CC) $(LIBS) -c semaphore.cpp

clean_queue:
	sh remover_filas.sh

clean:
	rm -f execution_manager
	rm -f hello_world
	rm -f scheduler
	rm -f at
	rm -f shutdown
	rm -f hypercube
