ROOT = ..

include $(ROOT)/common/Makefile.common

BINS = main-lf-linkedlist main-lf-skiplist main-hashtable main-lf-bst 

PROF = $(ROOT)/src

BUILDIR = .
TEST_FILE = main.cc

INC = ../../Libnvram/include
OBJ = ../../Libnvram/epoch.o ../../Libnvram/link-cache.o ../../Libnvram/active-page-table.o ../common/timehelper.o ../common/vsv.o

.PHONY:	all clean

all:	$(BINS)

../common/vsv.o: ../common/vsv.cc ../common/vsv.h
	g++ -o ../common/vsv.o -c ../common/vsv.cc -ltbb -ggdb -O0 -g

../common/timehelper.o: ../common/timehelper.cc ../common/timehelper.h
	g++ -o ../common/timehelper.o -c ../common/timehelper.cc

measurements.o:
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/measurements.o $(PROF)/measurements.c -I$(INC) -ggdb

lf-linkedlist.o:
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/lf-linkedlist.o ../src/linkedlist/lf-linkedlist.cpp -I$(INC) -ggdb

lf-skiplist.o:
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/lf-skiplist.o ../src/skiplist/lf-skiplist.cpp -I$(INC) -ggdb

lf-bst.o: 
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/lf-bst.o ../src/bst/lf-bst.cpp -I$(INC) -ggdb

linkedlist.o:
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/linkedlist.o ../src/hashtable/lists/lf-linkedlist.cpp -I$(INC) -ggdb

hashtable.o: ../src/hashtable/lists/lf-linkedlist.h linkedlist.o
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/hashtable.o ../src/hashtable/hashtable.cpp -I$(INC) -ggdb

intset.o: ../src/hashtable/lists/lf-linkedlist.h hashtable.o 
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/intset.o ../src/hashtable/intset.cpp -I$(INC) -ggdb

test-lf-linkedlist.o: ../common/timehelper.o ../common/vsv.o lf-linkedlist.o 
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/test-lf-linkedlist.o -DLINKEDLIST_T $(TEST_FILE) -I$(INC) -ggdb

test-lf-skiplist.o: ../common/timehelper.o ../common/vsv.o lf-skiplist.o 
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/test-lf-skiplist.o -DSKIPLIST_T $(TEST_FILE) -I$(INC) -ggdb

test-lf-bst.o: ../common/timehelper.o ../common/vsv.o lf-bst.o 
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/test-lf-bst.o -DBST_T $(TEST_FILE) -I$(INC) -ggdb

test-hashtable.o: ../common/timehelper.o ../common/vsv.o linkedlist.o intset.o hashtable.o
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/test-hashtable.o -DHASHTABLE_T $(TEST_FILE) -I$(INC) -ggdb

main-lf-linkedlist: measurements.o lf-linkedlist.o test-lf-linkedlist.o
	$(CC) $(CFLAGS) $(OBJ) -ltbb $(BUILDIR)/measurements.o $(BUILDIR)/lf-linkedlist.o $(BUILDIR)/test-lf-linkedlist.o -o main-lf-linkedlist $(LDFLAGS) -I$(INC) -ggdb

main-lf-skiplist: measurements.o lf-skiplist.o test-lf-skiplist.o
	$(CC) $(CFLAGS) $(OBJ) -ltbb $(BUILDIR)/measurements.o $(BUILDIR)/lf-skiplist.o $(BUILDIR)/test-lf-skiplist.o -o main-lf-skiplist $(LDFLAGS) -I$(INC) -ggdb

main-lf-bst: measurements.o lf-bst.o test-lf-bst.o
	$(CC) $(CFLAGS) $(OBJ) -ltbb $(BUILDIR)/measurements.o $(BUILDIR)/lf-bst.o $(BUILDIR)/test-lf-bst.o -o main-lf-bst $(LDFLAGS) -I$(INC) -ggdb

main-hashtable: measurements.o linkedlist.o hashtable.o intset.o test-hashtable.o 
	$(CC) $(CFLAGS) $(OBJ) -ltbb $(BUILDIR)/measurements.o $(BUILDIR)/linkedlist.o $(BUILDIR)/hashtable.o $(BUILDIR)/intset.o $(BUILDIR)/test-hashtable.o -o main-hashtable $(LDFLAGS) -I$(INC) -ggdb

clean:
	-rm -f $(BINS) $(BUILDIR)/test-lf-linkedlist.o $(BUILDIR)/test-lf-skiplist.o $(BUILDIR)/test-lf-bst.o $(BUILDIR)/test-hashtable.o $(BUILDIR)/lf-linkedlist.o $(BUILDIR)/lf-skiplist.o $(BUILDIR)/lf-bst.o $(BUILDIR)/linkedlist.o $(BUILDIR)/hashtable.o $(BUILDIR)/intset.o ../common/timehelper.o ../common/vsv.o 
