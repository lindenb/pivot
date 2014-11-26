CC=g++
CFLAGS:=-O3 -Wall

.PHONY:all clean


all: pivot

pivot : pivot.cpp pivot.hh githash.h
	$(CC) $(CFLAGS) -o $@ $<

githash.h:
	echo "#ifndef GIT_HASH" > $@
	echo -n '#define GIT_HASH "' >> $@
	-git rev-parse HEAD | tr -d "\n" >> $@
	echo '"' >> $@
	echo '#endif' >> $@


clean:
	rm -f pivot githash.h

