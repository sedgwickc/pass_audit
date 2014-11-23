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
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include "pass_audit.h"
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

int buff_init(int nw){

	num_workers = (int)nw;
	
	sem_init( &empty, 0, S_BBUFF ); // S_BBUFF buffers are empty to begin with...
	sem_init( &full, 0, 0 ); //  0 are full
	sem_init( &mutex, 0, 1 ); // mutex=1 because it is a lock

	buffer = malloc( sizeof ( char * ) * S_BBUFF );
	assert( buffer != NULL );

	for( int i = 0; i < S_BBUFF; i++){
		buffer[i] = malloc( sizeof( char *) * N_HASHES );
		assert( buffer[i] != NULL );

		for(int j = 0; j < N_HASHES; j++ ){
			buffer[i][j] = calloc( S_HASH, sizeof( char ) );
		}
	}

	workers = malloc( sizeof(Thread*) * num_workers );
	assert( workers != NULL );
	for( int i = 0; i < num_workers; i++ ){
		workers[i] = malloc( sizeof(Thread) );
		assert( workers[i] != NULL );

		workers[i]->num_hashes = 0;
	}
	
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
	assert( data != NULL);
	Hashes_Copy( &buffer[fillptr], data );
	numfill++;
	fillptr = (fillptr + 1) % S_BBUFF;
}
// buffer not getting hashes?
void buff_get(Hashes *data){
	assert( data != NULL);
	Hashes_Copy( data, &buffer[useptr] );
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

//race condition: consumers get stuck waiting
void *consume(void *arg){

	Hashes hashes = NULL;
	Hashes_Init( &hashes );	

	while( 1 ){
		sem_wait ( &full );
		sem_wait ( &mutex );
		buff_get( &hashes );
		sem_post ( &mutex );
		if( pdone > 0 )//strncmp( hashes[0], "DONE", S_HASH ) == 0)
		{
			Hashes_Free( &hashes );		
			return NULL;
		}
		buff_proc( hashes );
		workers[(long)arg]->num_hashes++;
#ifdef DEBUG
		printf("t_index: %ld, hash_no: %d\n", (long)arg, workers[(long)arg]->num_hashes);
#endif
		sem_post( &empty );

	}
	Hashes_Free( &hashes );		
	return NULL;
}

void buff_proc( Hashes data ){
	for( int i = 0; i < N_HASHES; i++ ){
		printf("%s: ", data[i] );
		// calc hash and compare, print password if found
		printf("\n");
	}
}

void buff_pdone(){
	Hashes done = NULL;
	Hashes_Init( &done );

	for( int i = 0; i < N_HASHES; i++ ){
		strncpy(done[i], "DONE", S_HASH);
	}

	sem_wait( &empty );
	for(int i = 0; i < S_BBUFF; i++){
		produce( &done );
		sem_post( &full );
	}

	sem_wait( &mutex );
	pdone++;
	sem_post( &mutex );

	Hashes_Free( &done );
}

void buff_free(){
	
	sem_destroy( &full );
	sem_destroy( &empty );
	sem_destroy( &mutex );

	for( int i = 0; i < S_BBUFF; i++){
		for(int j = 0; j < N_HASHES; j++ ){
			free( buffer[i][j] );
		}
		free( buffer[i] );
	}
	free( buffer );

	for( int i = 0; i < num_workers; i++ ){
		free(workers[i]);
	}
	
	free( workers );
}
