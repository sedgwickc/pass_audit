#ifndef CIRC_BUFF_H
#define CIRC_BUFF_H

/* Charles Sedgwick
 * 1684807
 * References:
 * http://beej.us/guide/bgipc/output/html/multipage/semaphores.html
 */
//#define _XOPEN_SOURCE 699 
#define S_BBUFF 50
#define S_FPATH 64
#define N_WORKERS 5

#include <semaphore.h>
#include <unistd.h>
#include <pthread.h>

/* 
 * buffer elements are a set number N_HASHES of hashes
 *
 */

typedef struct {
	int turn;
	int ticket;
}Tic_Lock;

typedef struct {
	pthread_t thread_id;
	size_t bytes_read;
	char hashes[N_HASHES][S_HASH];
}Thread;

extern Hashes *buffer;
extern Thread **threads;
extern int fillptr;
extern int useptr;
extern int numfill;

extern sem_t empty; 
extern sem_t full; 
extern sem_t mutex;
extern sem_t mutex_tic;

int buff_init(int num_workers);

pthread_t buff_add_worker(long index);

void buff_fill( Hashes *f );

void buff_get( Hashes *data, int *turn );

void produce(Hashes *data);

void *consume( void * );

void buff_proc( Hashes *f, int *turn );

void buff_pdone();

void buff_free();

void buff_tic_init();

int buff_tic_turn();

void buff_tic_lock( int *t );

void buff_tic_unlock();

int fetch_add( int *ptr );

#endif
