CC = gcc
CFLAGS = -Wall -iquote include -iquote lib -O3
DBGFLAGS = -Wall -iquote include -iquote lib -Og -g

OBJS_ := main.o argsparse.o dataparse.o cJSON.o
OBJS := $(addprefix obj/,$(OBJS_))
DBGOBJS := $(addprefix obj/debug/,$(OBJS_))

.PHONY : clean install all

all : bin/release/raincoat bin/debug/raincoat

# Release build rules

bin/release/raincoat : $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

obj/main.o : src/main.c src/apikey.h include/argsparse.h include/dataparse.h
	$(CC) $(CFLAGS) -c -o $@ $<

obj/argsparse.o : src/argsparse.c include/argsparse.h
	$(CC) $(CFLAGS) -c -o $@ $<

obj/dataparse.o : src/dataparse.c include/dataparse.h lib/cjson/cJSON.h
	$(CC) $(CFLAGS) -c -o $@ $<

obj/cJSON.o : lib/cjson/cJSON.c lib/cjson/cJSON.h
	$(CC) $(CFLAGS) -c -o $@ $<

# Debug build rules

bin/debug/raincoat : $(DBGOBJS)
	$(CC) $(DBGFLAGS) -o $@ $^

obj/debug/main.o : src/main.c src/apikey.h include/argsparse.h include/dataparse.h
	$(CC) $(DBGFLAGS) -c -o $@ $<

obj/debug/argsparse.o : src/argsparse.c include/argsparse.h
	$(CC) $(DBGFLAGS) -c -o $@ $<

obj/debug/dataparse.o : src/dataparse.c include/dataparse.h lib/cjson/cJSON.h
	$(CC) $(DBGFLAGS) -c -o $@ $<

obj/debug/cJSON.o : lib/cjson/cJSON.c lib/cjson/cJSON.h
	$(CC) $(DBGFLAGS) -c -o $@ $<

# Clean all built files

clean :
	rm bin/release/raincoat obj/*.o bin/debug/raincoat obj/debug/*.o

# Install in /usr/local/bin

install :
	cp --verbose bin/release/raincoat /usr/local/bin/

