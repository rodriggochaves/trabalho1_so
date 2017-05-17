// chave da fila de comunicação com os gerenciadores de execução
// EMS = executions managers
// os ultimos dois digitos definem o numero da fila (aresta)
#define QUEUE_KEY_EMS 1320000
#define QUEUE_KEY_FIRST_EM 1320000

#include <iostream>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <bitset>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

void listen_queues( std::vector<int> queue_em_ids ) {
  struct message {
    long pid;
    char program_name[30];
  };
  struct message received_msg;
  received_msg.pid = NULL;
  int number_of_queues = queue_em_ids.size();
  int counter = 0;

  std::cout << "Numero de filas: " << number_of_queues << std::endl;

  // while(1) {
  //   counter = 0
  //   while( counter < number_of_queues ) {
  //     sleep(100)
  //     msgrcv( queue_em_ids[counter], &received_msg, sizeof(received_msg), 0, MSG_NOERROR );
  //     if ( received_msg.pid ) break;
  //     counter += 1;
  //     if ( counter > )
  //   }
  //   std::cout << "Recebi: " << received_msg.program_name << std::endl;
  // }
}

std::vector<std::bitset<4>> identifies_neighbors(std::bitset<4> main) {
  std::vector<std::bitset<4>> neighbors;
  for (int i = 0; i < 4; ++i) {
    std::bitset<4> copied_main = main;
    copied_main[i] = 1 -  copied_main[i];
    neighbors.push_back(copied_main);
  }
  return neighbors;
}

int queue_key_number(std::string number) {
  // converte string para hexadecimal
  std::stringstream str;
  int value;

  str << number;
  str >> std::hex >> value;

  // retorna chave em função em_id
  return QUEUE_KEY_EMS;
}

int main(int argc, char const *argv[]) {
  int id_queue_em;
  int em_id;
  int queue_keys[4];
  int temp;
  std::vector<int> queue_em_ids[4];
  std::bitset<4> em_id_bit;
  std::vector<std::bitset<4>> neighbors;

  // recebe seu numero
  if (argc != 2) {
    std::cout << "Numero incorreto de argumentos" << std::endl;
    exit(1);
  }

  em_id = std::atoi(argv[1]);
  if (em_id == 0) {
    queue_em_ids[4] = msgget( QUEUE_KEY_FIRST_EM, 0777 );
  }
  em_id_bit = std::bitset<4>(em_id);
  // cria vizinhos
  neighbors = identifies_neighbors(em_id_bit);
  // converte numero dos vizinhos para inteiro
  for (int i = 0; i < 4; ++i) {
    temp = neighbors[i].to_ulong();
    if (em_id > temp) {
      queue_keys[i] = QUEUE_KEY_EMS + (em_id * 100) + temp;
    } else {
      queue_keys[i] = QUEUE_KEY_EMS + (temp * 100) + em_id;
    }
    // std::cout << queue_keys[i] << ": " << std::hex << queue_keys[i] << std::endl;
    queue_em_ids[i] = msgget( queue_keys[i], IPC_CREAT | 0777 );
  }

  listen_queues( queue_em_ids );

  // cria chave da fila
  // cria fila com constante + receptor(em decimal) + emissor(em decimal)
  // std::cout << "Eu obtenho a fila: " <<  << std::endl; 

  // recupera a fila em uso pelo trabalho
  // id_queue_em = msgget(QUEUE_KEY_EMS, 0777);
  // if( id_queue_em < 0 ) {
  //   std::cout << "error ao recuperar a fila" << std::endl;
  //   exit(1);
  // }

  // while(1) {
  //   // fica verificando se a mensagem chegou do #at
  //   response = msgrcv(id_queue_em, &rcvd_msg, sizeof(rcvd_msg), 0, 0);
  //   if (response < 0) {
  //     std::cout << "EM: Erro ao ler a mensagem" << std::endl;
  //   }

  //   if (! (std::strcmp(rcvd_msg.program_name,""))) {
  //     std::cout << "Recebi: " << rcvd_msg.program_name << std::endl;
  //     exit(0);
  //   }
  // }

  return 0;
}