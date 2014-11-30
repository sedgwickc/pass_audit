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

Dict *dict;

void Hashes_Init( Hashes *h )
{
	*h = malloc( sizeof( char *) * N_HASHES );

	for(int i = 0; i < N_HASHES; i++)
	{
		(*h)[i] = calloc(S_HASH, sizeof( char ) );
	}
}

void Hashes_Clear( Hashes *h )
{
	for( int i = 0; i < N_HASHES; i++ )
	{
		memset( (*h)[i], 0, S_HASH );
	}
}

void Hashes_Free( Hashes *h )
{
	for(int i = 0; i < N_HASHES; i++ )
	{
		free( (*h)[i] );
	}
	free( *h ); 
}

void Hashes_Copy( Hashes *dest, Hashes *src )
{
	for( int i = 0; i < N_HASHES; i++ )
	{
		strncpy( (*dest)[i], (*src)[i], S_HASH );
	}
}

void Pass_Crack( char *h_in )
{
	if( dict == NULL )
	{
		printf( "ERROR: Dictionary not initialized. Terminating...\n" );
		return;
	}
	else if( h_in == NULL || strlen( h_in ) == 0 )
	{
		printf( "ERROR: Hash empty or NULL.\n" );
		return;
	}

	char hash_out[BCRYPT_HASHSIZE + 1];
	char hash_in[BCRYPT_HASHSIZE + 1];
	int ret;
	
	memset( hash_out, '\0', BCRYPT_HASHSIZE + 1 );
	memset( hash_in, '\0', BCRYPT_HASHSIZE + 1 );
	strncpy( hash_in, h_in, BCRYPT_HASHSIZE );
	

	if( hash_in[1] == '1' )
	{

		char *openssl_cmd = "openssl passwd -1 -salt ";
		size_t s_cmd =  sizeof( openssl_cmd ) + S_HASH + S_WORD;
		char *popen_cmd = calloc( s_cmd, sizeof( char ) );
		char *salt = NULL, *save_ptr, *eol;
		FILE *fp = NULL;

		strtok_r( hash_in, "$", &save_ptr );
		salt = strtok_r( save_ptr, "$", &save_ptr );
		assert( salt != NULL );

		/* strtok_r removes the delimiter from the string you pass it causing
		 * the strncmp below to fail when the password is found thus hash_in has
		 * to be reset
		 */
		strncpy( hash_in, h_in, BCRYPT_HASHSIZE );
		for(long int i = 0; i < dict->num_words; i++ )
		{
			ret = snprintf(popen_cmd, s_cmd, "openssl passwd -1 -salt %s \"%s\"", 
					salt, dict->words[i]);
			assert( ret <= s_cmd );
			fp = popen( popen_cmd, "r");
			fread( hash_out, sizeof( char ), BCRYPT_HASHSIZE, fp );
			if( ( eol = strchr( hash_out, '\n' ) ) != NULL )
			{
				*eol = '\0';
			}
			ret = pclose( fp );
			assert( ret != -1 );
			
			if( strncmp( hash_in, hash_out, BCRYPT_HASHSIZE ) == 0 )
			{
				free( popen_cmd );
				printf( "%s:%s\n",hash_in, dict->words[i] );
				return;
			}
		}
	}
	else if( hash_in[1] == '2' && hash_in[2] == 'a' )
	{
		for(long int i = 0; i < dict->num_words; i++ )
		{
			ret = bcrypt_hashpw( dict->words[i], hash_in, hash_out );
			assert( ret == 0 );
			
			if( strncmp( hash_in, hash_out, BCRYPT_HASHSIZE ) == 0 )
			{
				printf( "%s:%s\n", hash_in, dict->words[i] );
				return;
			}
		}
	}
	
	printf( "%s:\n", hash_in );
}

void Dict_Init( char *file )
{
#ifdef DEBUG
	printf("Initializing Dictionary.\n");
#endif
	dict = malloc( sizeof( Dict ) );
	dict->num_words = N_WORDS;
	dict->words = malloc( N_WORDS * sizeof( char *) );
	assert( dict->words != NULL);
	Dict_Set( file );
}

void Dict_Set( char *file )
{
	if( dict == NULL )
	{
		printf("ERROR: Dictionary not initialized.\n");
		return;
	}

	int fd, status, offset = 0, word_ptrs, word_cnt;
	char *data_dict, word[S_WORD];
	void *tmp;
	struct stat s_dict;

	fd = open(file, O_RDONLY | O_RSYNC );
	assert( fd != -1);

	status = stat(file, &s_dict);
	assert( status != -1 );

	data_dict = mmap(0, s_dict.st_size, PROT_READ, MAP_SHARED, fd, 0);
	assert ( data_dict != MAP_FAILED );

	word_cnt = 0;
	word_ptrs = N_WORDS;

#ifdef DEBUG
	printf("Filling Dictionary with words from %s.\n", file);
#endif
	while(  sscanf(data_dict, "%s\n%n", word, &offset) != EOF )
	{
		data_dict += offset;
		
		if( word_cnt == word_ptrs - 1 )
		{
			tmp = realloc( dict->words, (word_ptrs + N_WORDS) * sizeof( char * ) );
			assert( tmp != NULL );
			dict->words = tmp;
			word_ptrs += N_WORDS; 
		}

		dict->words[word_cnt] = calloc( S_WORD + 1, sizeof( char ) );
		assert( dict->words[word_cnt] != NULL);
		strncpy( dict->words[word_cnt], word, S_WORD );
		word_cnt++;
		
	}
	dict->num_words = word_cnt;
}

Dict *Dict_Get()
{
	return dict;
}

void Dict_Free()
{
	if( dict == NULL )
	{
		printf("ERROR: Dictionary not initialized. Terminating...\n");
		return;
	}

	for( int i = 0; i < dict->num_words; i++ )
	{
		free( dict->words[i] );
	}
	free( dict->words );
	free( dict );
}
