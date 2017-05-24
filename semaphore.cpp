#include "semaphore.hpp"

struct sembuf operation[2];

// cria uma semaforo com a chave key
int criar_semaforo( int key ){
	int idsem;
	if (( idsem = semget( key, 1, IPC_CREAT | 0777 )) < 0) {
    printf("erro na criacao da fila\n");
    exit(1);
  }
 	return idsem;
}

// exclui a semaforo de id idfila
void excluir_semaforo( int idfila ) {
	semctl(idfila, 1, IPC_RMID, 0);
}

// up
void v_sem(int sem) {
    operation[0].sem_num = 0;
    operation[0].sem_op = 1;
    operation[0].sem_flg = 0;
    if (semop(sem, operation, 1) < 0)
    	printf("erro no p=%d\n", errno);
}

// down
void p_sem(int sem) {
     operation[0].sem_num = 0;
     operation[0].sem_op = -1;
     operation[0].sem_flg = 0;
     if ( semop(sem, operation, 1) < 0)
       printf("erro no p=%d\n", errno);
}
