#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sem.h>

// cria uma semaforo com a chave key
int criar_semaforo(int key);

// exclui a semaforo de id idfila
void excluir_semaforo(int idfila);

// realiza a operacao up
void v_sem(int sem);

// realiza a operacao down
void p_sem(int sem);
