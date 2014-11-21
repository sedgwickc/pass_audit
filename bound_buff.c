/* Charles Sedgwick
 * 1684807
 *
 * The following is an implementation of a bounded buffer using semaphores based
 * on the code found in Operating Systems: Three Easy Pieces
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>
#include <errno.h>
#include <openssl/sha.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include "bound_buff.h"
#include "memwatch.h"

Hashes *buffer;
Thread **workers;
int num_workers;
char *log_mess;
int fillptr;
int useptr; 
int numfill;
int pdone;

sem_t empty; 
sem_t full; 
sem_t mutex;

void hash_batch_copy( Hashes *src, Hashes *dest ){
	for( int i = 0; i < N_HASHES; i++ ){
		strncpy( dest[i], src[i], S_HASH );
	}
}

int buff_init(int nw){

	num_workers = (int)nw;
	
	sem_init(&empty, 0, S_BBUFF); // S_BBUFF buffers are empty to begin with...
	sem_init(&full, 0, 0); //  0 are full
	sem_init(&mutex, 0, 1); // mutex=1 because it is a lock

	buffer = malloc(sizeof(Hashes) * S_BBUFF );
	assert( buffer != NULL );

	workers = malloc( sizeof(Thread*) * num_workers );
	assert( workers != NULL );
	for( int i = 0; i < num_workers; i++ ){
		workers[i] = malloc( sizeof(Thread) );
		assert( workers[i] != NULL );

		workers[i]->num_files = 0;
	}
	
	log_mess = calloc(S_LOGMESS, sizeof(char) );

	fillptr = 0;
	useptr = 0;
	numfill = 0;
	pdone = 0;
	return 0;
}

pthread_t buff_add_worker(long index){
		pthread_t tid;
		pthread_create(&tid, NULL, consume, (void *)index);
		workers[index]->thread_id = tid;
		return tid;
}

void buff_fill(Hashes *data){
	for( int i = 0; i < N_HASHES; i++ ){
		strncpy( buffer[i], data, S_HASH );
	}
	numfill++;
	fillptr = (fillptr + 1) % S_BBUFF;
}

void buff_get(Hashes **data){
	assert( *data != NULL);
	hash_batch_copy( *data, buffer[useptr] );
	useptr = (useptr + 1) % S_BBUFF;
	numfill--;
}

void produce(Hashes *data){
	sem_wait( &empty );
	sem_wait( &mutex );
	buff_fill( data );
	sem_post( &mutex );
	sem_post( &full );
}

void *consume(void *arg){

	char t_id[20];
	char t_files[20];
	Hashes hashes = malloc(N_HASHES, sizeof( char *) );

	for(int i = 0; i < N_HASHES; i++){
		hashes[i] = calloc(S_HASH, sizeof( char ) );
	}

	while( 1 ){
		sem_wait ( &full );
		sem_wait ( &mutex );
		memset(file->filename, 0, S_FPATH );
		memset(file->checksum, 0, S_CHKSUM );
		buff_get(&hashes)
		sem_post ( &mutex );
		if( strncmp( file->filename, "DONE", S_FPATH ) == 0 ){
			if( file != NULL ){
				free( file->filename );
				free( file->checksum );
				free( file );
			}
			return NULL;
		}
		buff_proc( &hashes );
		workers[(long)arg]->num_hashes++;
#ifdef DEBUG
		printf("t_index: %ld, hash_no: %d\n", (long)arg, workers[(long)arg]->num_hashes);
#endif
		sem_post( &empty );

	}
	return NULL;
}

void buff_proc( Hashes *data){
	printf("processing hashes....");
	return;
}

void buff_pdone(){
	Hashes done;
	for( int i = 0; i < N_HASHES; i++ ){
		done[i] = "done";
	}

	sem_wait( &empty );
	for(int i = 0; i < S_BBUFF; i++){
		sem_wait( &mutex );
		produce(&done);
		sem_post( &mutex );
	}
	sem_post( &full );

	sem_wait( &mutex );
	pdone++;
	sem_post( &mustex );
}

void buff_free(){
	
	sem_destroy( full );
	sem_destroy( empty );
	sem_destroy( mutex );

	for(int i = 0; i < S_BBUFF; i++){
		free(buffer[i]->filename);
		free(buffer[i]->checksum);
		free(buffer[i]);
	}

	for( int i = 0; i < num_workers; i++ ){
		free(workers[i]->data->checksum);
		free(workers[i]->data->filename);
		free(workers[i]->data);
		free(workers[i]);
	}
	
	free( log_mess );
	free( workers );
	free( buffer );
}
