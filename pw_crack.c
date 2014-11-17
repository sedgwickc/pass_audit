/*
 * Charles Sedgwick
 * 1684807
 * sedgwickc
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "bound_buff.h"
#include "memwatch.h"

int main(int argc, char* argv[]){

	int num_workers;

//	pthread_t workers[num_workers];
//	buff_init(num_workers);
//	
//	for( int i = 0; i < num_workers; i++){
//		workers[i] = buff_add_worker((long)i);
//	}

	int fd_hashes, fd_dict, status, offset;
	char *hash = calloc ( S_HASH, sizeof( char ) );
	char *word = calloc ( S_WORD, sizeof( char ) );
	char *data_dict, *data_hash;
	struct stat s_dict, s_hashes;
	char *fn_dictionary = calloc ( S_FPATH, sizeof(char) );
	char *fn_hashes = calloc (S_FPATH, sizeof(char) );

	strncpy( fn_dictionary, argv[1], S_FPATH );
	strncpy( fn_hashes, argv[2], S_FPATH );

	fd_dict = open(fn_dictionary, O_RDONLY );
	assert( fd_dict != -1);

	fd_hashes = open(fn_hashes, O_RDONLY );
	assert( fd_hashes != -1);
	
	status = stat(fn_dictionary, &s_dict);
	assert( status != -1 );
	
	status = stat(fn_hashes, &s_hashes);
	assert( status != -1 );

	data_dict = mmap(0, s_dict.st_size, PROT_READ, MAP_SHARED, fd_dict, 0);
	assert ( data_dict != MAP_FAILED );
	
	data_hash = mmap( data_dict , s_hashes.st_size, PROT_READ, MAP_SHARED, fd_hashes, 0);
	assert ( data_hash != MAP_FAILED );

	while(  sscanf(data_hash, "%s\n%n", hash, &offset) != EOF ){
		data_hash += offset; 
#ifdef DEBUG
		printf("cur_hash: %s\n", hash);
#endif
		//produce(file);
	}

	offset = 0;
	while(  sscanf(data_dict, "%s\n%n", word, &offset) != EOF ){
		data_dict += offset; 
#ifdef DEBUG
		printf("cur_word: %s\n", word);
#endif
		//produce(file);
	}

//	buff_pdone();
	
//	for( int i = 0; i < num_workers; i++){
//		pthread_join(workers[i], (void *)&status );
//		if( status != 0 ){
//			printf("ERROR: %s\n", strerror( status ) );
//		}
//	}
//
	munmap( data_dict, s_dict.st_size + s_hashes.st_size ); 

	close(fd_dict);
	close(fd_hashes);
	free(fn_dictionary);
	free(fn_hashes);
	free(hash);
	free(word);
//	buff_free();
	return 0;
}
