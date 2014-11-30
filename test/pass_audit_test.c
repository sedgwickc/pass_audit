#define _GNU_SOURCE

#include <crypt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pass_audit.h"

int main(int argc, char *argv[] ){

	char *hash_in = "$2a$06$Hb.5z.pRpV9DMLpEnRWl4u8G0To1/bDEtYv.dl6FT1JKlId1r6XMq";

	Dict_Init( argv[1] );

	Pass_Crack( hash_in );

	hash_in = "$1$Dg21rtqv$ijUpZKYitPgoRbCkRb0cl1";

	Pass_Crack( hash_in );

	return 0;
}
