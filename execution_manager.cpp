// chave da fila de comunicação com os gerenciadores de execução
// EMS = executions managers
#define QUEUE_KEY_EMS 0x1928199

#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

int main(int argc, char const *argv[]) {
  int id_queue_em;
  int response;

  struct message {
    long pid;
    char program_name[30];
  };

  struct message rcvd_msg;

  // recupera a fila em uso pelo trabalho
  id_queue_em = msgget(QUEUE_KEY_EMS, 0777);
  if( id_queue_em < 0 ) {
    std::cout << "error ao recuperar a fila" << std::endl;
    exit(1);
  }

  while(1) {
    // fica verificando se a mensagem chegou do #at
    response = msgrcv(id_queue_em, &rcvd_msg, sizeof(rcvd_msg), 0, 0);
    if (response < 0) {
      std::cout << "EM: Erro ao ler a mensagem" << std::endl;
    }

    if (! (std::strcmp(rcvd_msg.program_name,""))) {
      std::cout << "Recebi: " << rcvd_msg.program_name << std::endl;
      exit(0);
    }
  }

  return 0;
}