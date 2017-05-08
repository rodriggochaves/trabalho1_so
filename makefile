# compilador
cc=clang++

# bibliotecas
LIBS = -std=c++11 -Wall

scheduler:
	$(CC) $(LIBS) -c scheduler.cpp
	$(CC) $(LIBS) -o main scheduler.cpp