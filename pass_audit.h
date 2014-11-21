#ifndef PASS_AUDIT_H
#define PASS_AUDIT_H

#define N_HASHES 20 
#define S_FPATH 64
#define S_HASH 64
#define S_WORD 80

typedef char Hashes[N_HASHES][S_HASH];

void compare( char *dict, char *hash );

#endif
