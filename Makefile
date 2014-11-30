CC=gcc

CFLAGS= -Wall -std=c99 -DMEMWATCH -DMW_STDIO -c

CFLAGS_DEBUG= -Wall -std=c99 -DMEMWATCH -DMW_STDIO -c -g -DDEBUG 

CFLAGS_LINK=-lcrypt -pthread

BCRYPT_FLAGS=-g -c -W -Wall -Wbad-function-cast -Wcast-align -Wcast-qual -Wmissing-prototypes -Wstrict-prototypes -Wshadow -Wundef -Wpointer-arith -O2 -fomit-frame-pointer -funroll-loops

all: pw_crack

bcrypt_hash: blowfish.o bcrypt.o bcrypt_hash.o bcrypt/bcrypt.a
	$(CC) $^ -o $@

bcrypt_verify: blowfish.o bcrypt.o bcrypt_verify.o bcrypt/bcrypt.a
	$(CC) $^ -o $@

debug: pw_crack_db.o pass_audit_db.o bound_buff_db.o memwatch.o blowfish.o bcrypt.o bcrypt/bcrypt.a 
	$(CC) $(CFLAGS_LINK) $^ -o pw_crack_debug

pw_crack: pw_crack.o pass_audit.o bound_buff.o memwatch.o blowfish.o bcrypt.o bcrypt/bcrypt.a
	$(CC) $(CFLAGS_LINK) $^ -o $@

bcrypt.o:
	$(CC) $(BCRYPT_FLAGS) bcrypt/bcrypt.c -o $@

blowfish.o:
	$(CC) $(BCRYPT_FLAGS) bcrypt/crypt_blowfish/crypt_blowfish.c -o $@

%_db.o: %.c
	$(CC) $(CFLAGS_DEBUG) -o $@ $^ 

%.o: %.c
	$(CC) $(CFLAGS) $^ 

clean:
	rm *.o pw_crack pw_crack_debug bcrypt_verify bcrypt_hash
