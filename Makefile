CC = gcc
CFLAGS = -std=c11 -g -Wall -pedantic -o
OBJS = client server 
HEADER = include
SRC = src

all: $(OBJS) supervisor

supervisor: supervisor_main.o supervisor_support.o
	$(CC) $(CFLAGS) $@.o $(SRC)/$@_main.o -pthread $(HEADER)/$@/$@_support.o $(HEADER)/hash_table/hash_table.o
	rm -f $(HEADER)/$@/$@_support.o $(SRC)/*.o $(HEADER)/hash_table/hash_table.o

$(OBJS):%: %_main.o %_support.o	
	$(CC) $(CFLAGS) $@.o $(SRC)/$< $(HEADER)/$@/$@_support.o
	rm -f $(HEADER)/$@/$@_support.o $(SRC)/*.o

supervisor_main.o :
	$(MAKE) -C $(HEADER)/hash_table
	$(MAKE) -C $(SRC)

%_main.o :
	$(MAKE) -C $(SRC)

%_support.o:
	$(MAKE) -C $(HEADER)/$(patsubst %_support.o,%,$@)/

clean:
	$(MAKE) -C $(HEADER)/client/ clean
	$(MAKE) -C $(HEADER)/server/ clean
	$(MAKE) -C $(HEADER)/supervisor/ clean
	$(MAKE) -C $(HEADER)/hash_table/ clean
	$(MAKE) -C $(SRC)/ clean
	rm -f *.o *.sock *.log *.send

test:
	sh test.sh
	bash misura.sh 20