#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "pass_audit.h"
#include "bcrypt/bcrypt.h"
#include "memwatch.h"

void compare( char *dict, char *hash_in ){

	char hash_out[BCRYPT_HASHSIZE];
	char pass[21];
	int ret;

	memset(pass, 0, 21);
	memset(hash_out, 0, BCRYPT_HASHSIZE);
	
	char *data_dict, word[S_WORD];
	int status, offset = 0;
	struct stat s_dict;
	int fd = open(dict, O_RDONLY );
	assert( fd != -1);

	status = stat(dict, &s_dict);
	assert( status != -1 );

	data_dict = mmap(0, s_dict.st_size, PROT_READ, MAP_SHARED, fd, 0);
	assert ( data_dict != MAP_FAILED );
	
	
	while(  sscanf(data_dict, "%s\n%n", word, &offset) != EOF ){
		data_dict += offset; 
		
		if( hash_in[1] == '1' ){
			//printf("\n");
			//strncpy(hash_out, "0",BCRYPT_HASHSIZE);
		}else if( hash_in[1] == '2' && hash_in[2] == 'a' ){

			ret = bcrypt_hashpw( word, hash_in, hash_out );
			assert( ret == 0 );
		}

		if( strncmp( hash_in, hash_out, BCRYPT_HASHSIZE ) == 0 ){
			printf("%s\n",word);
			munmap( data_dict, s_dict.st_size ); 
			close( fd );
			return;
		}
	}

	munmap( data_dict, s_dict.st_size ); 
	close( fd );

}
