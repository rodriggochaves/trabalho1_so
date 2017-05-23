// chave da fila de comunicação com os gerenciadores de execução
// EMS = executions managers
// os ultimos dois digitos definem o numero da fila (aresta)
#define QUEUE_KEY_EMS 1320000
#define QUEUE_KEY_FIRST_EM 1780000

#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <unistd.h>
#include <bitset>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

typedef struct message {
  long pid;
  char program_name[30];
  int seconds_to_wait;
  int destination;
} Message;

void listen_queues( int queue_em_ids[], int number_of_queues, int em_id ) {
  Message received_msg;
  received_msg.pid = NULL;
  int counter = 0;

  while(1) {
    while( counter < number_of_queues ) {
      sleep(5);
      msgrcv( queue_em_ids[counter], &received_msg, sizeof(received_msg), 0,
             IPC_NOWAIT );
      if ( received_msg.pid ){
        std::cout << "Nó  " << em_id << " Recebi: " << received_msg.program_name << std::endl;
        break;
      }
      counter += 1;
      if ( counter >= number_of_queues) counter = 0;
    }
  }
}

std::bitset<4> find_neighbour(int source, int destination) {
  std::bitset<4> source_bit = std::bitset<4>(source);
  std::bitset<4> destination_bit = std::bitset<4>(destination);
  std::bitset<4> next = source_bit ^ destination_bit;
  // std::cout << " Source: " << source_bit.to_string<char>() << " Destino: " << destination_bit.to_string<char>() << " Next: " << next.to_string<char>() << std::endl;
  int index = 0;
  for(int i = 0; i < 4; i++) {
    if(next[i] == 1) {
      index = i;
      break;
    }
  }
  std::bitset<4> masked = std::bitset<4>(0);
  masked[index] = 1;
  // std::cout << "source: " << source_bit << " destino:" << destination_bit << " masked: " << masked.to_string<char>() << std::endl;
  return masked | source_bit;
}

void handle_message(std::map<int,int> neighbours_map, struct message* message, int source) {
  std::bitset<4> neighbour = find_neighbour(source, message->destination);
  std::cout << "selected neighbour: " << neighbour.to_string<char>() << std::endl;
  msgsnd(neighbours_map[neighbour.to_ulong()], message, sizeof(*message) , IPC_NOWAIT);
}

std::vector<std::bitset<4>> identifies_neighbors(std::bitset<4> main) {
  std::vector<std::bitset<4>> neighbors;
  for (int i = 0; i < 4; ++i) {
    std::bitset<4> copied_main = main;
    copied_main[i] = 1 - copied_main[i];
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
  int em_id;
  int queue_keys[4];
  int temp;
  int queue_em_ids[5];
  int number_of_queues = 0;
  std::bitset<4> em_id_bit;
  std::vector<std::bitset<4>> neighbors;

  struct message received_msg;

  // recebe seu numero
  if (argc != 2) {
    std::cout << "Numero incorreto de argumentos" << std::endl;
    exit(1);
  }

  // id do gerente de execução corrente
  em_id = std::atoi(argv[1]);

  // se estiver executando o gerente 0, ganha acesso a fila junto com o
  // escalanador
  if (em_id == 0) {
    queue_em_ids[4] = msgget( QUEUE_KEY_FIRST_EM, 0777 );
    number_of_queues += 1;
  }
  // daqui não trava => mentira Uring!

  // id do gerente atual em bits
  em_id_bit = std::bitset<4>(em_id);

  // cria vizinhos
  neighbors = identifies_neighbors(em_id_bit);

  // std::cout << "Eu sou " << em_id << "\t";

  // para cada vizinho, cria a key da fila
  // (000)       (00)                     (00)
  // constante + maior id de um vertice + menor id do vertice
  std::map<int,int> neighbours_map;
  for (int i = 0; i < 4; ++i) {
    temp = neighbors[i].to_ulong();
    if (em_id > temp) {
      queue_keys[i] = QUEUE_KEY_EMS + (em_id * 100) + temp;
    } else {
      queue_keys[i] = QUEUE_KEY_EMS + (temp * 100) + em_id;
    }
    queue_em_ids[i] = msgget( queue_keys[i], IPC_CREAT | 0777 );
    neighbours_map[temp] = queue_em_ids[i];
    // printf("%d\t", queue_keys[i]);
    // std::cout << "---" << std::hex << queue_keys[i] << std::endl;
    number_of_queues += 1;
  }

  // ouve as filas esperando a mensagem
  if (em_id == 0) {
    queue_em_ids[4] = msgget( QUEUE_KEY_FIRST_EM, 0777 );
    number_of_queues += 1;

    while(1) {
      msgrcv(queue_em_ids[4], &received_msg, sizeof(received_msg), 0, 0);
      std::cout << "Recebi: " << received_msg.program_name << "destino: "<< received_msg.destination<< std::endl;
      handle_message(neighbours_map, &received_msg, em_id);
    }
  }

  listen_queues( queue_em_ids, number_of_queues, em_id );

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

  // remove a fila (scheduler <-> EM 0)
  msgctl(queue_em_ids[4], IPC_RMID, NULL);

  return 0;
}
