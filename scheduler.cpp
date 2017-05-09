#define QUEUE_KEY 0x1332624

#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

int main(int argc, char const *argv[]) {
  int pid;
  int id_fila;

  if (argc < 3 || argc > 3 ) {
    std::cout << "Quantidade de argumentos inválida" << std::endl;
    exit(1);
  }

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

  if (pid != 0) {
    std::cout << "Eu sou o pai" << std::endl;
  }
  
  // fazer esperar pelo tempo passado no segundo argumento

  // comunicar o programa a ser executado
  
  // TODO: fazer o shutdown apagar a fila do sistema
  
  return 0;
}