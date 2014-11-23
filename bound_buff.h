#ifndef CIRC_BUFF_H
#define CIRC_BUFF_H

/* Charles Sedgwick
 * 1684807
 * References:
 * http://beej.us/guide/bgipc/output/html/multipage/semaphores.html
 */
#define _XOPEN_SOURCE 699 
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
	pthread_t thread_id;
	size_t bytes_read;
	int num_hashes;
	char hashes[N_HASHES][S_HASH];
}Thread;

extern Hashes *buffer;
extern Thread **threads;
extern int num_workers;
extern int fillptr;
extern int useptr;
extern int numfill;

extern sem_t empty; 
extern sem_t full; 
extern sem_t mutex;

int buff_init(int num_workers);

pthread_t buff_add_worker(long index);

void buff_fill( Hashes *f );

void buff_get( Hashes *data );

void produce(Hashes *data);

void *consume( void * );

void buff_proc( Hashes *f);

void buff_pdone();

void buff_free();

#endif
