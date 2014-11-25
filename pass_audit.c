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

void Pass_Crack( char *hash_in )
{
	if( dict == NULL )
	{
		printf( "ERROR: Dictionary not initialized. Terminating...\n" );
		return;
	}
	else if( strlen( hash_in ) == 0 )
	{
		return;
	}

	char hash_out[BCRYPT_HASHSIZE];
	char *md5_res;
	struct crypt_data md5_data;
	int ret;
	memset(hash_out, 0, BCRYPT_HASHSIZE);
	
	md5_res = NULL;
	md5_data.initialized = 0;
	printf( "%s:", hash_in );
	for(long int i = 0; i < dict->num_words; i++ )
	{
		if( hash_in[1] == '1' )
		{
			/*
			md5_res = crypt_r( word, hash_in, &md5_data );
			assert( md5_res != NULL );
			printf("%s\n", md5_res);
			memset( hash_out, 0, BCRYPT_HASHSIZE );
			strncpy( hash_out, md5_res, BCRYPT_HASHSIZE );
			*/
			strncpy( hash_out, "filler", BCRYPT_HASHSIZE );
		}
		else if( hash_in[1] == '2' && hash_in[2] == 'a' )
		{
			ret = bcrypt_hashpw( dict->words[i], hash_in, hash_out );
			assert( ret == 0 );
		}
		if( strncmp( hash_in, hash_out, BCRYPT_HASHSIZE ) == 0 )
		{
			printf( "%s\n", dict->words[i] );
			return;
		}
	}
	printf( "\n" );
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

		dict->words[word_cnt] = calloc( S_WORD, sizeof( char ) );
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
