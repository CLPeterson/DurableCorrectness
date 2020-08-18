ROOT = ../..

include $(ROOT)/common/Makefile.common

BINS = $(BINDIR)/lf-skiplist
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

lf-skiplist.o: 
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/lf-skiplist.o lf-skiplist.cpp -I$(INC)

test.o: lf-skiplist.h lf-skiplist.o timehelper.o vsv.o
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/test.o $(TEST_FILE) -I$(INC)

main: measurements.o lf-skiplist.o test.o 
	$(CC) $(CFLAGS) $(OBJ) -ltbb $(BUILDIR)/measurements.o $(BUILDIR)/lf-skiplist.o  $(BUILDIR)/test.o -o $(BINS) $(LDFLAGS) -I$(INC)

clean:
	-rm -f $(BINS)
