CC = gcc
CFLAGS = -Wall -iquote include -iquote src -iquote lib -O3

OBJS_ := main.o argsparse.o dataparse.o cJSON.o
OBJS := $(addprefix obj/,$(OBJS_))

bin/release/raincoat : $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

obj/main.o: src/main.c src/apikey.h include/argsparse.h include/dataparse.h
	$(CC) $(CFLAGS) -c -o $@ $<

obj/argsparse.o: src/argsparse.c include/argsparse.h
	$(CC) $(CFLAGS) -c -o $@ $<

obj/dataparse.o: src/dataparse.c include/dataparse.h lib/cjson/cJSON.h
	$(CC) $(CFLAGS) -c -o $@ $<

obj/cJSON.o: lib/cjson/cJSON.c lib/cjson/cJSON.h
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean

clean:
	rm bin/release/raincoat obj/*.o

.PHONY: install

install:
	cp --verbose bin/release/raincoat /usr/local/bin/
