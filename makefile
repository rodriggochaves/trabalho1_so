# compilador
CC=clang++

# bibliotecas
LIBS = -std=c++11 -Wall

scheduler:
	$(CC) $(LIBS) -o scheduler scheduler.cpp

execution_manager:
	$(CC) $(LIBS) -o execution_manager execution_manager.cpp

hello_world:
	$(CC) $(LIBS) hello_world.cpp	-o hello_world 

clean:
	rm -f execution_manager
	rm -f hello_world
	rm -f scheduler
