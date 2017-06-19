#define STATUS_TABLE 0x199122
#define STATUS_TABLE_SEM 0x7392871
#define JOB_TABLE_ADDRESS_KEY 1201372
#define JOB_TABLE_SIZE 100

// chave da fila de comunicação com os gerenciadores de execução
// EMS = executions managers
// os ultimos dois digitos definem o numero da fila (aresta)
#define QUEUE_KEY_EMS 1320000
#define QUEUE_KEY_FIRST_EM 1780000

#include "semaphore.hpp"
#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <unistd.h>
#include <bitset>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <errno.h>
#include <sys/msg.h>
#include <string.h>
#include <time.h>

typedef struct job {
  int job;
  char exec_file[30];
  int seconds_to_wait;
  time_t start_time;
  time_t finish_time;
} Job;

typedef struct message {
  long pid;
  char program_name[30];
  int seconds_to_wait;
  int destination;
  int job_number;
} Message;

void handle_message(std::map<int,int> neighbours_map, struct message* message, int source);
std::bitset<4> find_neighbour(int source, int destination);
void exec_file(char*, int, int );
Job* get_job_entry(int job_number);

void prepare_to_die(int i) {
  exit(1);
}

void listen_queues(std::map<int,int> neighbours_map, int queue_em_ids[], int number_of_queues, int em_id) {
  Message received_msg;
  received_msg.pid = NULL;
  int counter = em_id;
  while( counter + 1 ) {
    sleep(2);
    for (auto const& neighbour : neighbours_map) {
      if ( msgrcv( neighbour.second, &received_msg, sizeof(received_msg), 0, IPC_NOWAIT ) >= 0 ) {
        // std::cout << "EM: " << counter << received_msg.program_name << std::endl;
        handle_message(neighbours_map, &received_msg, em_id);
      } else {
        msgrcv( neighbour.second, &received_msg, sizeof(received_msg), 0, IPC_NOWAIT );
      }
    }
  }
}

std::bitset<4> find_neighbour(int source, int destination) {
  std::bitset<4> source_bit = std::bitset<4>(source);
  std::bitset<4> destination_bit = std::bitset<4>(destination);
  std::bitset<4> next = source_bit ^ destination_bit;
  int index = 0;
  for(int i = 0; i < 4; i++) {
    if(next[i] == 1) {
      index = i;
      break;
    }
  }
  std::bitset<4> masked = std::bitset<4>(0);
  masked[index] = 1;
  return masked | source_bit;
}

void handle_message(std::map<int,int> neighbours_map, struct message* message, int source) {
  std::bitset<4> neighbour = find_neighbour(source, message->destination);
  if(source != message->destination) {
    // encaminha mensagem para o proximo nodo afim
    // de achar destinatário
    msgsnd(neighbours_map[neighbour.to_ulong()], message, sizeof(*message) , IPC_NOWAIT);
  } else {
    // encaminha para executar o programa
    exec_file(message->program_name, source, message->job_number);
  }
}

void change_em_status(int status, int em_identifier) {
  int status_table_id;
  int status_table_semaphore_id;

  // recupera o id da memoria da tabela de ocupado
  status_table_id = shmget( STATUS_TABLE, 16 * sizeof(int), 0700 );
  if (status_table_id < 0) {
    std::cout << "ERRO AO CRIAR A FILA: " << errno << std::endl;
  }
  status_table_semaphore_id = create_semaphore( STATUS_TABLE_SEM );
  // inicio da area de segurança
  p_sem(status_table_semaphore_id);
  int* status_table = (int *) shmat(status_table_id, 0, 0777);
  // 1 = ocupado
  status_table[em_identifier] = status;
  // for (int i = 0; i < 16; i++) {
  //   std::cout << "EM: " << i << " ==> " << status_table[i] << std::endl;
  // }
  if (shmdt(status_table) < 0) {
    std::cout << "Erro a desabilitar a memoria" << std::endl;
  }
  v_sem(status_table_semaphore_id);
  // fim da area de seguranca
}

void exec_file(char* program_name, int em_identifier, int job_number) {
  char file_path[100];
  int status;
  int pid;
  strcpy(file_path, "./");
  strcat(file_path, program_name);
  // time_t start_time = time(NULL);

  // std::cout << "Tempo de início: " << ctime(&start_time) << std::endl;
  pid = fork();
  if (pid == 0) {
    change_em_status(1, em_identifier);
    execl(file_path, program_name, NULL);
  }
  wait(&status);
  time_t end_time = time(NULL);
  change_em_status(0, em_identifier);

  if(em_identifier == 15) {
    Job* entry = get_job_entry(job_number);
    std::cout << "Job number: " << entry->job << std::endl;
    std::cout << "Arquivo: " << entry->exec_file << std::endl;
    std::cout << "Delay: " << entry->seconds_to_wait << std::endl;
    std::cout << "Hora de Início: " << ctime(&entry->start_time) << std::endl;
    entry->finish_time = end_time;
    std::cout << "Hora de Fim: " << ctime(&entry->finish_time) << std::endl;
    std::cout << "Makespan: " << entry->finish_time - entry->start_time << std::endl;
  }

}

Job* get_job_entry(int job_number) {
  int job_table_id = shmget( JOB_TABLE_ADDRESS_KEY, JOB_TABLE_SIZE * sizeof( Job* ), 0700);
  Job* job_table = (Job*) shmat(job_table_id, 0, 0700);
  return &job_table[job_number];
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

  // recebe seu numero
  if (argc != 2) {
    std::cout << "Numero incorreto de argumentos" << std::endl;
    exit(1);
  }

  // sinal para derrota
  signal(SIGTERM, prepare_to_die);

  // id do gerente de execução corrente
  em_id = std::atoi(argv[1]);

  // se estiver executando o gerente 0, ganha acesso a fila junto com o
  // escalanador
  std::map<int,int> neighbours_map;
  if (em_id == 0) {
    queue_em_ids[4] = msgget( QUEUE_KEY_FIRST_EM, 0700 );
    if (queue_em_ids[4] < 0) {
      std::cout << "erro" << std::endl;
    }
    neighbours_map[17] = queue_em_ids[4];
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
    // while(1) {
    //   msgrcv(queue_em_ids[4], &received_msg, sizeof(received_msg), 0, 0);
    //   std::cout << "Recebi: " << received_msg.program_name << "destino: "<< received_msg.destination<< std::endl;
      // handle_message(neighbours_map, &received_msg, em_id);
    // }
  }
  listen_queues(neighbours_map, queue_em_ids, number_of_queues, em_id );

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
