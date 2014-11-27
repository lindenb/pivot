CC=g++
CFLAGS:=-O3 -Wall

.PHONY:all clean test


all: pivot

pivot : pivot.o leveldb/libleveldb.a
	$(CC) $(CFLAGS) -o $@ $^ -lpthread


pivot.o : pivot.cpp pivot.hh githash.h leveldb/libleveldb.a
	$(CC) $(CFLAGS) -c -I leveldb/include -o $@ $< 

githash.h:
	echo "#ifndef GIT_HASH" > $@
	echo -n '#define GIT_HASH "' >> $@
	-git rev-parse HEAD | tr -d "\n" >> $@
	echo '"' >> $@
	echo '#endif' >> $@

leveldb/libleveldb.a: leveldb
	$(MAKE) -C leveldb

leveldb:
	git clone "https://github.com/google/leveldb.git"

test: pivot
	./pivot pivot.hh

clean:
	rm -rf pivot githash.h *.o leveldb

