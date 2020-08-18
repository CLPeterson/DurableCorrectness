ROOT = ../..

include $(ROOT)/common/Makefile.common

ALGO_O = herlihy.o
ALGO_C = herlihy.c

BINS = "$(BINDIR)/lb-sl_herlihy$(BIN_SUFFIX)"
PROF = $(ROOT)/src

INC = ../../../libnvram/include
OBJ = ../../../libnvram/epoch.o ../../../libnvram/link-cache.o ../../../libnvram/active-page-table.o

.PHONY:	all clean

all:	main

measurements.o:
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/measurements.o $(PROF)/measurements.c -I$(INC)

skiplist-lock.o: 
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/skiplist-lock.o skiplist-lock.c -I$(INC)

herlihy.o: skiplist-lock.h skiplist-lock.o 
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/herlihy.o herlihy.c -I$(INC)

intset.o: skiplist-lock.h herlihy.h 
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/intset.o intset.c -I$(INC)

test.o: skiplist-lock.h herlihy.h intset.h
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/test.o $(TEST_FILE) -I$(INC)

main: measurements.o skiplist-lock.o herlihy.o intset.o test.o 
	$(CC) $(CFLAGS) $(OBJ) $(BUILDIR)/measurements.o $(BUILDIR)/skiplist-lock.o $(BUILDIR)/herlihy.o $(BUILDIR)/intset.o $(BUILDIR)/test.o -o $(BINS) $(LDFLAGS) -I$(INC)

clean:
	rm -f $(BINS)
