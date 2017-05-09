#define QUEUE_KEY 0x1332624

#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

int main(int argc, char const *argv[]) {
  int pid;
  int id_fila;

  struct message {
    long pid;
    char program_name[30];
    int seconds_to_wait;
  };

  struct message at_message;

  // inicializa a fila com a chave do grupo
  id_fila = msgget(QUEUE_KEY, IPC_CREAT | 0777);
  if (id_fila < 0) {
    std::cout << "Erro ao criar a fila" << std::endl;
    exit(1);
  }

  // criar as 16 cópias dos gerênciadores de execução
  for (int i = 0; i < 16; ++i) {
    pid = fork();
    if (pid == 0) {
      execl("./execution_manager", "execution_manager", NULL);
      break;
    }
  }

  // loop infinito esperando nova chamada para executar o programa em X
  // tempos.
  // Por enquanto, vamos fazer um loop infinito que espera uma mensagem na
  // por uma mensagem com o programa a ser executado no tempo X
  if (pid != 0) {
    while(1) {
      msgrcv(id_fila, &at_message, sizeof(at_message), 0, 0);
      std::cout << "Scheduler: " << at_message.program_name << std::endl;
    }
  }
  
  // fazer esperar pelo tempo passado no segundo argumento

  // comunicar o programa a ser executado
  
  // TODO: fazer o shutdown apagar a fila do sistema
  
  return 0;
}