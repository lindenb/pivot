CC:=gcc
CFLAGS:=-O3 -Wall

.PHONY:all pivot


all: pivot

pivot : pivot.c
	$(CC) $(CFLAGS) -o $@ $<
	
clean:
	rm -f pivot

