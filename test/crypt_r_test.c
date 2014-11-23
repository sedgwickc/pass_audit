#define _GNU_SOURCE

#include <crypt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[] ){

	char *guess = "table";
	char *res = NULL;
	char *hash = argv[1];
	struct crypt_data data;
	data.initialized = 0;

	res = crypt_r( guess, hash, &data );

	printf( "res: %s\n", res );

	guess = "chair";
	char *guess2 = guess;
	char hash2[50];
	strncpy(hash2, hash, 50 );
	res = crypt_r( guess2, hash2, &data );

	printf( "res: %s\n", res );
	return 0;
}
