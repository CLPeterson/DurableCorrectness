ROOT = ../..

include $(ROOT)/common/Makefile.common

BINS = $(BINDIR)/lb-bst_tk
PROF = $(ROOT)/src

INC = ../../../libnvram/include
OBJ = ../../../libnvram/epoch.o ../../../libnvram/link-cache.o ../../../libnvram/active-page-table.o

.PHONY:	all clean

all:	main


measurements.o:
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/measurements.o $(PROF)/measurements.c -I$(INC)

bst.o: 
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/bst.o bst.c -I$(INC)

bst_tk.o: bst.h bst.o
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/bst_tk.o bst_tk.c -I$(INC)

intset.o: bst.h bst_tk.h
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/intset.o intset.c -I$(INC)

test.o: bst.h bst_tk.h intset.h
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/test.o $(TEST_FILE) -I$(INC)

main: measurements.o bst.o bst_tk.o intset.o test.o
	$(CC) $(CFLAGS) $(OBJ) $(BUILDIR)/measurements.o  $(BUILDIR)/bst.o $(BUILDIR)/bst_tk.o $(BUILDIR)/intset.o $(BUILDIR)/test.o -o $(BINS) $(LDFLAGS) -I$(INC)

clean:
	rm -f $(BINS)
