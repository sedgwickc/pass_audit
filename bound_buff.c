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
Tic_Lock *tic_print;
static int num_workers;
char *log_mess;
int fillptr;
int useptr; 
int numfill;

sem_t empty; 
sem_t full; 
sem_t mutex;
sem_t mutex_tic;

int buff_init(int nw){

	num_workers = (int)nw;
	
	sem_init( &empty, 0, S_BBUFF ); 
	sem_init( &full, 0, 0 );
	sem_init( &mutex, 0, 1 );
	sem_init( &mutex_tic, 0, 1 );

	buff_tic_init();

	buffer = malloc( sizeof ( Hashes ) * S_BBUFF );
	assert( buffer != NULL );

	for( int i = 0; i < S_BBUFF; i++){
		buffer[i] = malloc( sizeof( char * ) * N_HASHES );
		assert( buffer[i] != NULL );
		
		for(int j = 0; j < N_HASHES; j++ ){
			buffer[i][j] = calloc( S_HASH, sizeof( char ) );
			assert( buffer[i][j] != NULL );
		}
	}

	workers = malloc( sizeof(Thread*) * num_workers );
	assert( workers != NULL );
	for( int i = 0; i < num_workers; i++ ){
		workers[i] = malloc( sizeof(Thread) );
		assert( workers[i] != NULL );

	}
	
	fillptr = 0;
	useptr = 0;
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
	Hashes_Copy( &(buffer[fillptr]), data );
	fillptr = (fillptr + 1) % S_BBUFF;
}

void buff_get(Hashes *data, int *turn){
	assert( data != NULL);
	*turn = buff_tic_turn();
	Hashes_Copy( data, &buffer[useptr] );
	useptr = (useptr + 1) % S_BBUFF;
}

void produce(Hashes *data){
	sem_wait( &empty );
	sem_wait( &mutex );
	buff_fill( data );
	sem_post( &mutex );
	sem_post( &full );
}

void *consume(void *arg){

	int turn;

	Hashes hashes = NULL;
	Hashes_Init( &hashes );	

	while( 1 ){
		sem_wait ( &full );
		sem_wait ( &mutex );
		buff_get( &hashes, &turn );
		sem_post ( &mutex );
		sem_post( &empty );
		if( strncmp( hashes[0], "DONE", S_HASH ) == 0)
		{
			Hashes_Free( &hashes );		
			return NULL;
		}
		buff_proc( &hashes, &turn );
		Hashes_Clear( &hashes );
	}
	Hashes_Free( &hashes );		
	return NULL;
}

void buff_proc( Hashes *data, int *turn )
{
	assert( data != NULL );
	assert( turn != NULL );
	char *res = NULL;
	for( int i = 0; i < N_HASHES; i++ ){
		if( (*data)[i] != NULL && strlen( (*data)[i] ) != 0 ){
			Pass_Crack( (*data)[i], &res );
			assert( res != NULL );
			buff_tic_lock( turn );
			printf( "%s\n", res );
			buff_tic_unlock();
			free( res );
		}
	}
}

void buff_pdone()
{	
	Hashes done = NULL;
	Hashes_Init( &done );

	for( int i = 0; i < N_HASHES; i++ ){
		strncpy(done[i], "DONE", S_HASH);
	}

	for(int i = 0; i < S_BBUFF; i++){
		produce( &done );
	}

	Hashes_Free( &done );
}

void buff_free()
{
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
	free( tic_print );
	free( workers );
}

void buff_tic_init()
{
	tic_print = malloc( sizeof( Tic_Lock ) );
	assert( tic_print != NULL );
	tic_print->turn = 0;
	tic_print->ticket = 0;
}

int buff_tic_turn()
{
	int my_turn = fetch_add( &tic_print->ticket );
	return my_turn;
}

void buff_tic_lock(  int *t )
{
	while ( *t != tic_print->turn )
		;
}

void buff_tic_unlock()
{
	fetch_add( &tic_print->turn );
}

int fetch_add( int *ptr )
{
	sem_wait( &mutex_tic );
	int old = *ptr;
	*ptr = old + 1;
	sem_post( &mutex_tic );

	return old;
}
