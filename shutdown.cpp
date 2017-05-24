// status table shared memory area
#define STATUS_TABLE 0x199122
#define STATUS_TABLE_SEM 0x7392871

#define JOB_TABLE_ADDRESS_KEY 1201372
#define JOB_TABLE_SIZE 100

// chave da area de memoria compartilhada para controle dos pid's
#define SHARED_MEMORY 0x1991222

#define QUEUE_KEY_AT 0x1332624
#define QUEUE_KEY_FIRST_EM 1780000

#include "semaphore.hpp"
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

  // deleta semaforo da tabela de status
  int semaphore_id = create_semaphore( STATUS_TABLE_SEM );
  delete_semaphore( semaphore_id );

  // libera memoria
  int sm_id = shmget( STATUS_TABLE, 16 * sizeof(int), 0700 );
  shmctl(sm_id, IPC_RMID, NULL);
  // libera memoria
  sm_id = shmget( JOB_TABLE_ADDRESS_KEY , JOB_TABLE_SIZE * sizeof(int), 0700 );
  shmctl(sm_id, IPC_RMID, NULL);

  int keys[] = { 0x001424a4, 0x00142508, 0x001425d0, 0x00142760, 0x001424a4,
   0x0014256d, 0x00142635, 0x001427c5, 0x0014256e, 0x0014256d, 0x001426ff,
   0x0014288f, 0x0014256e, 0x00142508, 0x0014269a, 0x0014282a, 0x00142638,
   0x0014269c, 0x001425d0, 0x001428f4, 0x00142638, 0x00142701, 0x00142635,
   0x00142959, 0x00142702, 0x00142701, 0x001426ff, 0x00142a23, 0x001427cc,
   0x00142830, 0x001428f8, 0x00142760, 0x001427cc, 0x00142895, 0x0014295d,
   0x001427c5, 0x00142896, 0x00142830, 0x001429c2, 0x0014282a, 0x00142960,
   0x001429c4, 0x001428f8, 0x001428f4, 0x00142896, 0x00142895, 0x00142a27,
   0x0014288f, 0x00142960, 0x00142a29, 0x0014295d, 0x00142959, 0x00142a2a,
   0x001429c4, 0x001429c2, 0x001429be, 0x00142a2a, 0x00142a29, 0x00142a27,
   0x00142a23, 0x00142702, 0x0014269c, 0x0014269a, 0x001429be };

  int id;

  for(auto k : keys) {
    id = msgget(k, 0777);
    if (id < 0) {
      std::cout << "Não recuperei a fila " << k << std::endl;
    } else {
      std::cout << "Fila: " << id << std::endl;
      msgctl(id, IPC_RMID, NULL);
    }
  }

  return 0;
}
