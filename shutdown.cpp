#define QUEUE_KEY_AT 0x1332624
#define QUEUE_KEY_FIRST_EM 1780000

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>

int main(int argc, char const *argv[]) {
  int id_queue_at;
  int id_queue_em_0;

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

  // apaga fila do AT <-> scheduler
  msgctl(id_queue_at, IPC_RMID, NULL);
  // apaga fila do scheduler <-> EM 0
  msgctl(id_queue_em_0, IPC_RMID, NULL);

  return 0;
}
