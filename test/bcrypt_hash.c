#include "bcrypt/bcrypt.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>

int main( int argc, char** argv ){

	char salt[BCRYPT_HASHSIZE];
	char hash[BCRYPT_HASHSIZE];
	char pass[21];
	int ret;

	memset(pass, 0, 21);
	
	if( argc != 2 ){
		printf("Usage: bcrypt_hash PASSWORD\n");
		return 1;
	}

	strncpy( pass, argv[1], 20);

	ret = bcrypt_gensalt( 12, salt );
	assert( ret == 0 );

	ret = bcrypt_hashpw( pass, salt, hash );
	assert( ret == 0 );

	printf( "%s\n", hash);

	return 0;
}
