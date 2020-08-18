ROOT = ../..

include $(ROOT)/common/Makefile.common

ifeq ($(RO_FAIL),0)
	CFLAGS += -DRO_FAIL=0
	SUFFIX = _no_ro
else
	CFLAGS += -DRO_FAIL=1
endif

BINS = $(BINDIR)/lb-ll_lazy
PROF = $(ROOT)/src

INC = ../../../libnvram/include
OBJ = ../../../libnvram/epoch.o ../../../libnvram/link-cache.o ../../../libnvram/active-page-table.o

.PHONY:	all clean

all:	main

measurements.o:
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/measurements.o $(PROF)/measurements.c -I$(INC)

linkedlist-lock.o: 
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/linkedlist-lock.o linkedlist-lock.cpp -I$(INC)

lazy.o: linkedlist-lock.h linkedlist-lock.o 
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/lazy.o lazy.cpp -I$(INC)

intset.o: linkedlist-lock.h lazy.h 
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/intset.o intset.cpp -I$(INC)

test.o: linkedlist-lock.h lazy.h intset.h
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/test.o $(TEST_FILE) -I$(INC)

main: measurements.o linkedlist-lock.o lazy.o intset.o test.o 
	$(CC) $(CFLAGS) $(OBJ) $(BUILDIR)/measurements.o $(BUILDIR)/linkedlist-lock.o $(BUILDIR)/lazy.o $(BUILDIR)/intset.o $(BUILDIR)/test.o -o $(BINS) $(LDFLAGS) -I$(INC)

clean:
	rm -f $(BINS)
