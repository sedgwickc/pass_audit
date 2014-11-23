#ifndef PASS_AUDIT_H
#define PASS_AUDIT_H

#define N_HASHES 20 
#define S_FPATH 64
#define S_HASH 64
#define S_WORD 80
#define N_WORDS 1000

typedef char** Hashes;

typedef struct {
	char **words;
	size_t num_words;
}Dict;

void Hashes_Init( Hashes *h );
void Hashes_Free( Hashes *h );
void Hashes_Copy( Hashes *src, Hashes *dest );
void Pass_Crack( char *hash );
void Dict_Init( char *file );
void Dict_Set( char *file );
Dict *Dict_Get();
void Dict_Free();
#endif
