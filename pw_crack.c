/*
 * Charles Sedgwick
 * 1684807
 * sedgwickc
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <getopt.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <pthread.h>
#include "pass_audit.h"
#include "bound_buff.h"
#include "memwatch.h"

/* 
 * could use a tickets lock to ensure order of output
 *
 * handle num_workers cmdline arg
 */

int main(int argc, char* argv[])
{
	int fd_hashes, status, offset, i, ret;
	int num_workers = 0, opt;
	char *hash = calloc ( S_HASH, sizeof( char ) );
	char *data_hash;
	struct stat s_hashes;
	char *fn_dictionary = calloc ( S_FPATH, sizeof(char) );
	char *fn_hashes = calloc (S_FPATH, sizeof(char) );
	Hashes hashes = NULL;
	Hashes_Init( &hashes );
	
	/* getopt code based off of example code found in getopt man page */
	while( (opt = getopt(argc, argv, "n:")) != -1 ){
		switch (opt){
			case 'n':
				if( (num_workers = atoi(optarg)) > 0 ){
#ifdef DEBUG
					printf("Running with %d workers.", num_workers );
#endif
				} else {
					printf("Cannot have 0 workers\n");
					return 1;
				}
				break;
			default:
				break;
		}
	}

	if( num_workers <= 0 )
	{
		num_workers = N_WORKERS;
	}
	
	pthread_t workers[num_workers];
	buff_init( num_workers );
	for( int j = 0; j < num_workers; j++)
	{
		workers[j] = buff_add_worker( (long)j );
	}
	
	Dict_Init( argv[optind] );

	strncpy( fn_hashes, argv[optind + 1], S_FPATH );

	fd_hashes = open(fn_hashes, O_RDONLY );
	assert( fd_hashes != -1);
	
	status = stat(fn_hashes, &s_hashes);
	assert( status != -1 );
	
	data_hash = mmap( 0 , s_hashes.st_size, PROT_READ, MAP_SHARED, fd_hashes, 0);
	assert ( data_hash != MAP_FAILED );

	i = 0;
	ret = sscanf(data_hash, "%s\n%n", hash, &offset);
	while( ret != EOF )
	{
		data_hash += offset; 

		if( i < N_HASHES )
		{
			strncpy(hashes[i], hash, S_HASH);
			i++;
		}

		ret = sscanf(data_hash, "%s\n%n", hash, &offset);

		if( ret == EOF || i >= N_HASHES )
		{
			i = 0;
			produce( &hashes );
			Hashes_Clear( &hashes );
		}
	}

	buff_pdone();
  
	for( int i = 0; i < num_workers; i++)
	{
		pthread_join(workers[i], (void *)&status );
		if( status != 0 )
		{
			printf("ERROR: %s\n", strerror( status ) );
		}
	}

	munmap( data_hash, s_hashes.st_size ); 

	free( fn_dictionary );
	free( fn_hashes );
	free( hash );
	Hashes_Free( &hashes );
	Dict_Free();
	buff_free();
	return 0;
}
