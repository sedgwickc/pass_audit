#include "../bcrypt/bcrypt.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>

int main( int argc, char *argv[] ){

	char* hash_in;
	char hash_out[BCRYPT_HASHSIZE];
	char pass[21];
	int ret;

	memset(pass, 0, 21);
	memset(hash_out, 0, BCRYPT_HASHSIZE);
	
	if( argc != 2 ){
		printf("Usage: bcrypt_hash PASSWORD\n");
		return 1;
	}

	strncpy( pass, argv[1], 20);
	
	hash_in = "$2a$12$EepYTZTtUXlclRDfqKLWd.4Zzj4UpPqjP/uTQeoLAPD8qBNSbte16"
	
	ret = bcrypt_hashpw( pass, hash_in, hash_out );
	assert( ret == 0 );

	if( strncmp( hash_in, hash_out, BCRYPT_HASHSIZE ) == 0 ){
		printf("The password matches.\n");
	}else{
		printf("The password DOES NOT match.\n");
	}

	return 0;
}
