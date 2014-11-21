#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <crypt.h>
#include "pass_audit.h"
#include "bcrypt/bcrypt.h"
#include "memwatch.h"

void compare( char *dict, char *hash_in ){

	char hash_out[BCRYPT_HASHSIZE];
	char pass[21];
	char *data_dict, *md5_res, word[S_WORD];
	struct crypt_data md5_data;
	int fd, ret, status, offset = 0;
	struct stat s_dict;
	fd= open(dict, O_RDONLY );
	assert( fd != -1);

	memset(pass, 0, 21);
	memset(hash_out, 0, BCRYPT_HASHSIZE);
	
	status = stat(dict, &s_dict);
	assert( status != -1 );

	data_dict = mmap(0, s_dict.st_size, PROT_READ, MAP_SHARED, fd, 0);
	assert ( data_dict != MAP_FAILED );
	
	md5_res = NULL;
	while(  sscanf(data_dict, "%s\n%n", word, &offset) != EOF ){
		data_dict += offset; 
		
		if( hash_in[1] == '1' ){
			md5_data.initialized = 0;
			printf("word: %s, hash_in: %s\n", word, hash_in);
			md5_res = crypt_r( word, hash_in, &md5_data );
			assert( md5_res != NULL );
			md5_res = crypt( "test", "$1$G9tXgHBN$ZsSy3hyxQ4sz74oppF9WP0");
			printf("%s\n", md5_res);
			memset( hash_out, 0, BCRYPT_HASHSIZE );
			strncpy( hash_out, md5_res, BCRYPT_HASHSIZE );
		}else if( hash_in[1] == '2' && hash_in[2] == 'a' ){

			ret = bcrypt_hashpw( word, hash_in, hash_out );
			assert( ret == 0 );
		}

		if( strncmp( hash_in, hash_out, BCRYPT_HASHSIZE ) == 0 ){
			printf( "%s\n", word );
			munmap( data_dict, s_dict.st_size ); 
			close( fd );
			return;
		}

		memset( word, 0, S_WORD );
	}

	munmap( data_dict, s_dict.st_size ); 
	close( fd );

}
