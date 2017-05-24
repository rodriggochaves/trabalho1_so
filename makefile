# compilador
CC=clang++

# bibliotecas
LIBS = -std=c++11 -Wall

all: clean semaphore at scheduler execution_manager hello_world shutdown clean_queue

scheduler:
	$(CC) $(LIBS) -c escalonador.cpp
	$(CC) $(LIBS) -o escalonador semaphore.o escalonador.o

execution_manager:
	$(CC) $(LIBS) -c gerente_execucao.cpp
	$(CC) $(LIBS) -o gerente_execucao semaphore.o gerente_execucao.o

at:
	$(CC) $(LIBS) -o executa_postergado executa_postergado.cpp

hello_world:
	$(CC) $(LIBS) -o hello_world hello_world.cpp

shutdown:
	$(CC) $(LIBS) -c shutdown.cpp
	$(CC) $(LIBS) -o shutdown semaphore.o shutdown.o

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
