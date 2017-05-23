// chave da area de memoria compartilhada para controle dos pid's
#define SHARED_MEMORY 0x1991222

#define QUEUE_KEY_AT 0x1332624
#define QUEUE_KEY_FIRST_EM 1780000

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <csignal>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>

int main(int argc, char const *argv[]) {
  int id_queue_at;
  int id_queue_em_0;
  int memory_id;
  int* pid_pointer;

  // inicializa a AT <-> scheduler
  id_queue_at = msgget(QUEUE_KEY_AT, 0777);
  if (id_queue_at < 0) {
    std::cout << "Não recuperei a fila <AT <---> scheduler>" << std::endl;
  }

  // inicializa a fila scheduler <-> EM 0
  id_queue_em_0 = msgget(QUEUE_KEY_FIRST_EM, 0777);
  if (id_queue_em_0 < 0) {
    std::cout << "Não recuperei a fila <scheduler <---> EM 0>" << std::endl;
  }

  // recupera acesso a area de memoria compartilhada
  // recupera o id da memoria
  memory_id = shmget( SHARED_MEMORY, sizeof(int), 0777 );
  // salva o pid do processo do escalanador na memoria compartilhada
  pid_pointer = (int *) shmat(memory_id, 0, 0777);
  // std::cout << "Estou derrotando: " << *pid_pointer << std::endl;
  kill(*pid_pointer, SIGTERM);


  // apaga fila do AT <-> scheduler
  msgctl(id_queue_at, IPC_RMID, NULL);
  // apaga fila do scheduler <-> EM 0
  msgctl(id_queue_em_0, IPC_RMID, NULL);

  return 0;
}
