// chave da fila de comunicação com o At
#define QUEUE_KEY_AT 0x1332624

// chave da fila de comunicação com os gerenciadores de execução
// EMS = executions managers
#define QUEUE_KEY_EMS 0x1928199

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

// funcao que converte um int para ponteiro para char
const char * convert_id(int i) {
  std::string s = std::to_string( i );
  return s.c_str();
}

int main(int argc, char const *argv[]) {
  int pid;
  int id_queue_at;
  int id_queue_ems;

  struct message {
    long pid;
    char program_name[30];
    int seconds_to_wait;
  };

  struct message at_message, ems_message;

  // inicializa a fila com o at's
  id_queue_at = msgget(QUEUE_KEY_AT, IPC_CREAT | 0777);
  if (id_queue_at < 0) {
    std::cout << "Erro ao criar a fila 'AT'" << std::endl;
    exit(1);
  }

  // inicializa a fila com os em's
  id_queue_ems = msgget(QUEUE_KEY_EMS, IPC_CREAT | 0777);
  if (id_queue_ems < 0) {
    std::cout << "Erro ao criar a fila 'EMS'" << std::endl;
    exit(1); 
  }

  // criar as 16 cópias dos gerênciadores de execução
  for (int i = 0; i < 16; ++i) {
    pid = fork();
    if (pid == 0) {
      execl("./execution_manager", "execution_manager", convert_id(i), NULL);
      break;
    }
  }

  // loop infinito esperando nova chamada para executar o programa em X
  // tempos.
  // Por enquanto, vamos fazer um loop infinito que espera uma mensagem na
  // por uma mensagem com o programa a ser executado no tempo X
  // if (pid != 0) {
  //   while(1) {
  //     // fica verificando se a mensagem chegou do #at
  //     msgrcv(id_queue_at, &at_message, sizeof(at_message), 0, 0);

  //     // espera o tempo para executar
  //     std::cout << "Esperando: " << at_message.seconds_to_wait << std::endl;
  //     sleep(at_message.seconds_to_wait);

  //     // manda mensagem
  //     std::strcpy(ems_message.program_name, at_message.program_name);
  //     msgsnd(id_queue_ems, &ems_message, sizeof(ems_message), 0);
  //   }
  // }
  
  // fazer esperar pelo tempo passado no segundo argumento

  // comunicar o programa a ser executado
  
  // TODO: fazer o shutdown apagar a fila do sistema
  
  return 0;
}