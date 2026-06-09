CC = gcc
CFLAGS = -Wall -Wextra -iquote include -iquote lib -O2
DBGFLAGS = -Wall -Wextra -iquote include -iquote lib -Og -g

OBJS_ := main.o arg_parser.o data_parser.o cJSON.o
OBJS := $(addprefix obj/,$(OBJS_))
DBGOBJS := $(addprefix obj/debug/,$(OBJS_))

.PHONY : clean install all release debug

release : bin/release/raincoat

debug : bin/debug/raincoat

all : bin/release/raincoat bin/debug/raincoat

# Release build rules

bin/release/raincoat : $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

obj/main.o : src/main.c src/api_key.h include/arg_parser.h include/data_parser.h
	$(CC) $(CFLAGS) -c -o $@ $<

obj/arg_parser.o : src/arg_parser.c include/arg_parser.h
	$(CC) $(CFLAGS) -c -o $@ $<

obj/data_parser.o : src/data_parser.c include/data_parser.h lib/cjson/cJSON.h
	$(CC) $(CFLAGS) -c -o $@ $<

obj/cJSON.o : lib/cjson/cJSON.c lib/cjson/cJSON.h
	$(CC) $(CFLAGS) -c -o $@ $<

# Debug build rules

bin/debug/raincoat : $(DBGOBJS)
	$(CC) $(DBGFLAGS) -o $@ $^

obj/debug/main.o : src/main.c src/api_key.h include/arg_parser.h include/data_parser.h
	$(CC) $(DBGFLAGS) -c -o $@ $<

obj/debug/arg_parser.o : src/arg_parser.c include/arg_parser.h
	$(CC) $(DBGFLAGS) -c -o $@ $<

obj/debug/data_parser.o : src/data_parser.c include/data_parser.h lib/cjson/cJSON.h
	$(CC) $(DBGFLAGS) -c -o $@ $<

obj/debug/cJSON.o : lib/cjson/cJSON.c lib/cjson/cJSON.h
	$(CC) $(DBGFLAGS) -c -o $@ $<

# Clean all built files

clean :
	rm bin/release/raincoat obj/*.o bin/debug/raincoat obj/debug/*.o

# Install in /usr/local/bin

install :
	cp --verbose bin/release/raincoat /usr/local/bin/

