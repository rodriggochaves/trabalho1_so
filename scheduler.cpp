// chave da area de memoria compartilhada para controle dos pid's
#define SHARED_MEMORY 0x1991222

// chave da fila de comunicação com o At
#define QUEUE_KEY_AT 0x1332624

// chave da fila de comunicação com os gerenciadores de execução
// EMS = executions managers
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

// variaveis globais
int sons_pid[16];

void prepare_to_die(int i) {
  std::cout << "preparando para encerrar!" << std::endl;

  for (int i = 0; i < 16; i++) {
    kill(sons_pid[i], SIGKILL);
  }

  int status;
  // aguardar a derrota dos gerentes
  while(wait(&status) != -1);

  exit(1);
}

// funcao que converte um int para ponteiro para char
const char * convert_id(int i) {
  std::string s = std::to_string( i );
  return s.c_str();
}

int main(int argc, char const *argv[]) {
  int pid;
  int id_queue_at;
  int id_queue_em;
  int memory_id;
  int* pid_pointer;

  struct message {
    long pid;
    char program_name[30];
    int seconds_to_wait;
    int destination;
  };

  struct message at_message, ems_message;

  signal(SIGTERM, prepare_to_die);

  // recupera o id da memoria
  memory_id = shmget( SHARED_MEMORY, sizeof(int), IPC_CREAT | 0777 );
  // salva o pid do processo do escalanador na memoria compartilhada
  pid_pointer = (int *) shmat(memory_id, 0, 0777);
  *pid_pointer = getpid();

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
  for (int i = 0; i < 16; ++i) {
    pid = fork();
    if (pid == 0) {
      execl("./execution_manager", "execution_manager", convert_id(i), NULL);
      break;
    } else {
      sons_pid[i] = pid;
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
      std::strcpy(ems_message.program_name, at_message.program_name);
      ems_message.pid = getpid();
      // std::cout << "Enviando de " << ems_message.pid << std::endl;
      // std::cout << "Enviando para " << id_queue_em << std::endl;
      std::cout << "Enviando mensagem " << ems_message.program_name << std::endl;
      for(int i = 0; i < 16; i++) {
        ems_message.destination = i;
        std::cout << "escalanador: enviando mensagem para " << i << std::endl;
        msgsnd(id_queue_em, &ems_message, sizeof(ems_message), IPC_NOWAIT);
      }
    }
  }

  // fazer esperar pelo tempo passado no segundo argumento

  // comunicar o programa a ser executado

  // TODO: fazer o shutdown apagar a fila do sistema

  return 0;
}
