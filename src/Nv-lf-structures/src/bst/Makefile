ROOT = ../..

include $(ROOT)/common/Makefile.common

BINS = $(BINDIR)/lf-bst
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

lf-bst.o: 
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/lf-bst.o lf-bst.cpp -I$(INC)

test.o: lf-bst.h lf-bst.o timehelper.o vsv.o
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/test.o $(TEST_FILE) -I$(INC)

main: measurements.o lf-bst.o test.o 
	$(CC) $(CFLAGS) $(OBJ) -ltbb $(BUILDIR)/measurements.o $(BUILDIR)/lf-bst.o  $(BUILDIR)/test.o -o $(BINS) $(LDFLAGS) -I$(INC)

clean:
	-rm -f $(BINS) vsv.o timehelper.o
