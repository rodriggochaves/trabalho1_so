// chave da fila de comunicação com os gerenciadores de execução
// EMS = executions managers
// os ultimos dois digitos definem o numero da fila (aresta)
#define QUEUE_KEY_EMS 0x1320000

#include <iostream>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <bitset>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

std::vector<std::bitset<4>> identifies_neighbors(std::bitset<4> main) {
  std::cout << "Eu sou: " << main << "\t";
  std::vector<std::bitset<4>> neighbors;
  for (int i = 0; i < 4; ++i) {
    std::bitset<4> copied_main = main;
    copied_main[i] = 1 -  copied_main[i];
    neighbors.push_back(copied_main);
  }
  std::cout << "Meus vizinhos são: ";
  for (int i = 0; i < 4; ++i) {
    std::cout << neighbors[i] << " ";
  }
  std::cout << std::endl;
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
  int response;
  int em_id;
  std::bitset<4> em_id_bit;
  std::vector<std::bitset<4>> neighbors;

  struct message {
    long pid;
    char program_name[30];
  };

  struct message rcvd_msg;

  // recebe seu numero
  if (argc != 2) {
    std::cout << "Numero incorreto de argumentos" << std::endl;
    exit(1);
  }

  em_id = std::atoi(argv[1]);
  em_id_bit = std::bitset<4>(em_id);
  // cria vizinhos
  neighbors = identifies_neighbors(em_id_bit);
  // converte numero dos vizinhos para inteiro
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