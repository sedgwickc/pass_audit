CC=gcc

CFLAGS=-Wall -std=c99 -DMEMWATCH -DMW_STDIO -DMW_PTHREADS -c -D_POSIX_SOURCE

CFLAGS_DEBUG=-Wall -std=c99 -DMEMWATCH -DMW_STDIO -DMW_PTHREADS -c -g -D_POSIX_SOURCE -DDEBUG 

CFLAGS_LINK=-lcrypto -pthread

all: pw_crack

debug: pw_crack_debug

pw_crack_debug: pw_crack_db.o memwatch_db.o
	$(CC) $(CFLAGS_LINK) $^ -o $@

pw_crack: pw_crack.o memwatch.o
	$(CC) $(CFLAGS_LINK) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) $^ 

%_db.o: %.c
	$(CC) $(CFLAGS_DEBUG) -o $@ $^ 

clean:
	rm *.o pw_crack pw_crack_debug
