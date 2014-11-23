#ifndef PASS_AUDIT_H
#define PASS_AUDIT_H

#define N_HASHES 20 
#define S_FPATH 64
#define S_HASH 64
#define S_WORD 80

typedef char** Hashes;

void Hashes_Init( Hashes *h );
void Hashes_Free( Hashes *h );
void Hashes_Copy( Hashes *src, Hashes *dest );
void Hash_Compare( char *dict, char *hash );

#endif
