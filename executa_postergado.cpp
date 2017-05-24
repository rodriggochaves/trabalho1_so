#define QUEUE_KEY_AT 0x1332624

#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

int main(int argc, char const *argv[]) {
  int id_fila;

  typedef struct message {
    long pid;
    char program_name[30];
    int seconds_to_wait;
    int destination;
    int job_number;
  } Message;

  Message msg_to_send;

  // falha com a quantidade de argumentos errada
  if (argc < 3 || argc > 3 ) {
    std::cout << "Quantidade de argumentos inválida" << std::endl;
    exit(1);
  }

  // recupera a fila <AT <-> scheduler>
  id_fila = msgget(QUEUE_KEY_AT, 0777);
  if( id_fila < 0 ) {
    std::cout << "error ao recuperar a fila" << std::endl;
    exit(1);
  }

  // prepara a mensagem para colocar na fila
  msg_to_send.pid = getpid();
  std::strcpy(msg_to_send.program_name, argv[1]);
  msg_to_send.seconds_to_wait = std::stoi(argv[2]);

  // envia mensagem
  msgsnd(id_fila, &msg_to_send, sizeof(msg_to_send), 0);

  return 0;
}
