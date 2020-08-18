ROOT = ../..

include $(ROOT)/common/Makefile.common

BINS = $(BINDIR)/lf-hashtable
LLREP = lists
PROF = $(ROOT)/src

INC = ../../../libnvram/include
OBJ = ../../../libnvram/epoch.o ../../../libnvram/link-cache.o ../../../libnvram/active-page-table.o timehelper.o vsv.o

.PHONY:	all clean

all:	main

vsv.o: ../../common/vsv.cc ../../common/vsv.h
	g++ -o vsv.o -c ../../common/vsv.cc -ltbb -ggdb -O0 -g

timehelper.o: ../../common/timehelper.cc ../../common/timehelper.h
	g++ -o timehelper.o -c ../../common/timehelper.cc

measurements.o:
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/measurements.o $(PROF)/measurements.c -I$(INC)

linkedlist.o:
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/linkedlist.o $(LLREP)/lf-linkedlist.cpp -I$(INC)

hashtable.o: $(LLREP)/lf-linkedlist.h linkedlist.o
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/hashtable.o hashtable.cpp -I$(INC)

intset.o: $(LLREP)/lf-linkedlist.h hashtable.o 
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/intset.o intset.cpp -I$(INC)

test.o: linkedlist.o intset.o hashtable.o timehelper.o vsv.o
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/test.o $(TEST_FILE) -I$(INC)

main: measurements.o linkedlist.o intset.o hashtable.o test.o 
	$(CC) $(CFLAGS) $(OBJ) -ltbb $(BUILDIR)/measurements.o $(BUILDIR)/linkedlist.o $(BUILDIR)/hashtable.o $(BUILDIR)/intset.o $(BUILDIR)/test.o -o $(BINS) $(LDFLAGS) -I$(INC)

clean:
	-rm -f $(BINS)
