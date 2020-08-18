ROOT = ../..

include $(ROOT)/common/Makefile.common

ifeq ($(RO_FAIL),0)
	CFLAGS += -DRO_FAIL=0
	SUFFIX = _no_ro
else
	CFLAGS += -DRO_FAIL=1
endif

BINS  = $(BINDIR)/lb-ht_lazy$(BIN_SUFFIX)$(SUFFIX)
LLREP = lists
PROF = $(ROOT)/src

INC = ../../../libnvram/include
OBJ = ../../../libnvram/epoch.o ../../../libnvram/link-cache.o ../../../libnvram/active-page-table.o

.PHONY:	all clean

all:	main

measurements.o:
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/measurements.o $(PROF)/measurements.c -I$(INC)


intset.o:
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/intset.o $(LLREP)/intset.cpp -I$(INC)

lazy.o: intset.o 
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/lazy.o $(LLREP)/lazy.cpp -I$(INC)

linkedlist-lock.o: intset.o lazy.o 
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/linkedlist-lock.o $(LLREP)/linkedlist-lock.cpp -I$(INC)

hashtable-lock.o: intset.o lazy.o linkedlist-lock.o
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/hashtable-lock.o hashtable-lock.cpp -I$(INC)

test.o: intset.o lazy.o linkedlist-lock.o hashtable-lock.o
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/test.o $(TEST_FILE) -I$(INC)

main: measurements.o intset.o lazy.o linkedlist-lock.o hashtable-lock.o test.o 
	$(CC) $(CFLAGS) $(OBJ) $(BUILDIR)/measurements.o  $(BUILDIR)/intset.o $(BUILDIR)/lazy.o $(BUILDIR)/linkedlist-lock.o $(BUILDIR)/hashtable-lock.o $(BUILDIR)/test.o -o $(BINS) $(LDFLAGS) -I$(INC)

clean:
	rm -f $(BINS)


