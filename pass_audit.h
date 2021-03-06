#ifndef PASS_AUDIT_H
#define PASS_AUDIT_H

#include <stddef.h>

#define N_HASHES 1 
#define S_FPATH 64
#define S_HASH 64
#define S_WORD 80
#define S_SALT_MD5 8 
#define N_WORDS 1000

typedef char** Hashes;

typedef struct {
	char **words;
	size_t num_words;
}Dict;

void Hashes_Init( Hashes *h );
void Hashes_Clear( Hashes *h );
void Hashes_Free( Hashes *h );
void Hashes_Copy( Hashes *src, Hashes *dest );
void Pass_Crack( char *hash, char **res );
void Dict_Init( char *file );
void Dict_Fill( char *file );
Dict *Dict_Get();
void Dict_Free();
#endif
