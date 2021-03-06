// status table shared memory area
#define STATUS_TABLE 0x199122
#define STATUS_TABLE_SEM 0x7392871

// chave da area de memoria compartilhada para controle dos pid's
#define SHARED_MEMORY 0x1991222

// chave da fila de comunicação com o At
#define QUEUE_KEY_AT 0x1332624

// chave da fila de comunicação com os gerenciadores de execução
// EMS = executions managers
#define QUEUE_KEY_FIRST_EM 1780000

#define MESSAGE_ADDRESS_KEY 1201370
#define JOB_TABLE_ADDRESS_KEY 1201372
#define JOB_TABLE_SIZE 100

#include "semaphore.hpp"
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
#include <ctime>
#include <vector>
#include <sys/sem.h>

// variaveis globais
int sons_pid[16];

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

Message* init_message();
Job* init_job_table();
void run_delayed(int signal);

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
  std::vector<Job> execution_table;
  int status_table_id;
  int status_table_semaphore_id;

  struct message at_message;

  signal(SIGTERM, prepare_to_die);

  // recupera o id da memoria da tabela de ocupado
  status_table_id = shmget( STATUS_TABLE, 16 * sizeof( int ), IPC_CREAT | 0700 );
  if (status_table_id < 0) {
    std::cout << "Erro ao alocar STATUS_TABLE: " << status_table_id << std::endl;
    exit(1);
  }
  status_table_semaphore_id = create_semaphore( STATUS_TABLE_SEM );
  // inicio da area de segurança
  p_sem(status_table_semaphore_id);
  int* status_table = (int *) shmat( status_table_id, 0, 0777 );
  for (int i = 0; i < 16; i++)
    status_table[i] = 0;
  if (shmdt(status_table) < 0) {
    std::cout << "Erro a desabilitar a memoria" << std::endl;
  }
  v_sem(status_table_semaphore_id);
  // fim da area de seguranca


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
      execl("./gerente_execucao", "gerente_execucao", convert_id(i), NULL);
      break;
    } else {
      sons_pid[i] = pid;
    }
  }

  // loop infinito esperando nova chamada para executar o programa em X
  // tempos.
  // Por enquanto, vamos fazer um loop infinito que espera uma mensagem na
  // por uma mensagem com o programa a ser executado no tempo X
  int job_counter = 1;
  Job* job_table_ptr = init_job_table();
  if (pid != 0) {
    std::cout << "Esperando pela mensagem" << std::endl;
    while(1000 > 1) {
      // fica verificando se a mensagem chegou do #at
      if (msgrcv(id_queue_at, &at_message, sizeof(at_message), 0, 0) < 0) {
        // std::cout << "Erro ao ler a mensagem" << std::endl;
      } else {
        Job entry;
        entry.job = job_counter;
        entry.seconds_to_wait = at_message.seconds_to_wait;
        std::strcpy(entry.exec_file, at_message.program_name);
        // espera o tempo para executar
        if (at_message.pid != 0) {
          std::cout << "Esperando " << at_message.seconds_to_wait;
          std::cout << " para executar " << at_message.program_name << std::endl;
        }

        // sleep(at_message.seconds_to_wait);

        Message* message_ptr = init_message();
        // manda mensagem
        std::strcpy(message_ptr->program_name, at_message.program_name);
        message_ptr->seconds_to_wait = at_message.seconds_to_wait;
        message_ptr->pid = getpid();
        std::cout << "Enviando mensagem " << message_ptr->program_name << std::endl;
        entry.start_time = time(NULL);
        message_ptr->job_number = job_counter;
        job_table_ptr[job_counter] = entry;

        signal(SIGALRM, run_delayed);
        alarm(at_message.seconds_to_wait);

        //run_delayed(0);
        job_counter++;
      }
    }
  }

  return 0;
}

void run_delayed(int signal) {
  int id_queue_em = msgget( QUEUE_KEY_FIRST_EM, IPC_CREAT | 0777);
  if (id_queue_em < 0) {
    std::cout << "error ao recuperar a fila EM" << std::endl;
  }
  int message_id = shmget( MESSAGE_ADDRESS_KEY, sizeof(Message*), 0700);
  if (message_id < 0) {
    std::cout << "Erro em recuperar a memoria << run_delayed" << std::endl;
  }
  Message* message_ptr = (Message*) shmat(message_id, 0, 0777);

  for(int i = 0; i < 16; i++) {
    message_ptr->destination = i;
    // std::cout << "escalanador: enviando mensagem para " << i << std::endl;
    msgsnd(id_queue_em, message_ptr, sizeof(Message), IPC_NOWAIT);
  }
}

Message* init_message() {
  int message_id = shmget( MESSAGE_ADDRESS_KEY, sizeof(Message*), IPC_CREAT | 0700);
  if (message_id < 0) {
    std::cout << "Erro ao alocar a mensagem: " << message_id << std::endl;
    exit(1);
  }
  return (Message*) shmat(message_id, 0, 0700);
}

Job* init_job_table() {
  int job_table_id = shmget( JOB_TABLE_ADDRESS_KEY, JOB_TABLE_SIZE * sizeof( Job* ), IPC_CREAT | 0700);
  if (job_table_id < 0) {
    std::cout << "Erro ao alocar JOB_TABLE: " << job_table_id << std::endl;
    exit(1);
  }
  return (Job*) shmat(job_table_id, 0, 0700);
}
