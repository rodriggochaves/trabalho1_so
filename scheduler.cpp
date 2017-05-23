// chave da fila de comunicação com o At
#define QUEUE_KEY_AT 0x1332624

// chave da fila de comunicação com os gerenciadores de execução
// EMS = executions managers
#define QUEUE_KEY_FIRST_EM 1780000

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>

// funcao que converte um int para ponteiro para char
const char * convert_id(int i) {
  std::string s = std::to_string( i );
  return s.c_str();
}

int main(int argc, char const *argv[]) {
  int pid;
  int id_queue_at;
  int id_queue_em;

  struct message {
    long pid;
    std::string program_name;
    int seconds_to_wait;
    int destination;
  };

  struct message at_message, ems_message;

  // inicializa a fila com o at's
  id_queue_at = msgget(QUEUE_KEY_AT, IPC_CREAT | 0777);
  if (id_queue_at < 0) {
    std::cout << "Erro ao criar a fila 'AT'" << std::endl;
    exit(1);
  }

  // // inicializa a fila com o primeiro EM
  id_queue_em = msgget(QUEUE_KEY_FIRST_EM, IPC_CREAT | 0777);
  if (id_queue_em < 0) {
    std::cout << "Erro ao criar a fila 'QUEUE_KEY_FIRST_EM'" << std::endl;
    std::cout << "Erro numero => " << errno << std::endl;
    exit(1);
  }

  // criar as 16 cópias dos gerênciadores de execução
  for (int i = 0; i < 1; ++i) {
    pid = fork();
    if (pid == 0) {
      execl("./execution_manager", "execution_manager", convert_id(i), NULL);
      break;
    }
  }

  // while(1) {
  // }

  // loop infinito esperando nova chamada para executar o programa em X
  // tempos.
  // Por enquanto, vamos fazer um loop infinito que espera uma mensagem na
  // por uma mensagem com o programa a ser executado no tempo X
  if (pid != 0) {
    std::cout << "Esperando pela mensagem" << std::endl;
    while(1) {
      // fica verificando se a mensagem chegou do #at
      msgrcv(id_queue_at, &at_message, sizeof(at_message), 0, 0);

      // espera o tempo para executar
      if (at_message.pid != 0) {
        std::cout << "Esperando " << at_message.seconds_to_wait;
        std::cout << " para executar " << at_message.program_name << std::endl;
      }
      // sleep(at_message.seconds_to_wait);

      // manda mensagem
      ems_message.program_name = at_message.program_name;
      ems_message.pid = getpid();
      std::cout << "Enviando de " << ems_message.pid << std::endl;
      std::cout << "Enviando para " << id_queue_em << std::endl;
      std::cout << "Enviando mensagem " << ems_message.program_name << std::endl;
      for(int i = 0; i < 16; i++) {
        ems_message.destination = i;
        msgsnd(id_queue_em, &ems_message, sizeof(ems_message), 0);
      }
    }
  }

  // fazer esperar pelo tempo passado no segundo argumento

  // comunicar o programa a ser executado

  // TODO: fazer o shutdown apagar a fila do sistema
  msgctl(id_queue_at, IPC_RMID, NULL);

  return 0;
}
