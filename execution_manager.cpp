#define QUEUE_KEY 0x1332624

#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

int main(int argc, char const *argv[]) {
  int id_fila;

  // recupera a fila em uso pelo trabalho
  id_fila = msgget(QUEUE_KEY, 0777);
  if( id_fila < 0 ) {
    std::cout << "error ao recuperar a fila" << std::endl;
    exit(1);
  }

  std::cout << "Eu sou o filho que conseguiu a fila" << id_fila << std::endl;

  return 0;
}