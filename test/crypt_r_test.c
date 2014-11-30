#define _GNU_SOURCE

#include <crypt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

int main(int argc, char *argv[] ){

	char *guess = "table";
	char *guess_dyn = calloc( 20, 1 );
	char *res;
	char *hash;
	struct crypt_data data;
	data.initialized = 0;

	if( argc == 2 ) 
	{
		hash = argv[1];
	}
	else
	{
		hash = "$1$G9tXgHBN$ZsSy3hyxQ4sz74oppF9WP0";
	}

	strncpy( guess_dyn, guess, 20 );

	res = crypt( guess, hash );
	assert( res != NULL );	

	printf( "res: %s\n", res );

	guess = "chair";
	char *guess2 = guess;
	char hash2[50];
	strncpy(hash2, hash, 50 );
	res = crypt_r( guess2, hash2, &data );
	assert( res != NULL );	

	printf( "res: %s\n", res );
	return 0;
}
