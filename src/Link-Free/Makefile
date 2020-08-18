CFLAGS = -std=c++11 -fpermissive -O3
BUCKET_NUM = 128 #CORRECTNESS ANNOTATIONS
LFLAGS = -L./include -pthread -lssmem
LINKFREE = ./LinkFree
SOFT = ./SOFT
IFLAGS = -I./include -I$(LINKFREE) -I$(SOFT) -I. 
OBJ = common/vsv.o common/timehelper.o
#all: list hash sl main
all: sl main

common/vsv.o: common/vsv.cc common/vsv.h
	g++ -o common/vsv.o -c common/vsv.cc -ltbb -ggdb -O0 -g

common/timehelper.o: common/timehelper.cc common/timehelper.h
	g++ -o common/timehelper.o -c common/timehelper.cc

main: bench/main.cc $(OBJ)
	g++ bench/main.cc $(OBJ) -ltbb -std=c++14 -pthread $(CFLAGS) $(IFLAGS) $(LFLAGS) -DBUCKET_NUM=$(BUCKET_NUM) -o main -ggdb

#list: ListBench.cpp SOFT/SOFTList.h LinkFree/LinkFreeList.h include/BenchUtils.h $(OBJ)
#	make -C ./include all
#	g++ ListBench.cpp $(CFLAGS) $(IFLAGS) $(LFLAGS) $(OBJ) -ltbb -o list

#hash: HashBench.cpp SOFT/SOFTHashTable.h LinkFree/LinkFreeHashTable.h include/BenchUtils.h $(OBJ)
#	make -C ./include all
#	g++ HashBench.cpp $(CFLAGS) $(IFLAGS) $(LFLAGS) -DBUCKET_NUM=$(BUCKET_NUM) $(OBJ) -ltbb -o hash

sl: SLBench.cpp SOFT/SOFTSkipList.h LinkFree/LinkFreeSkipList.h include/BenchUtils.h $(OBJ)
	make -C ./include all
	g++ SLBench.cpp $(CFLAGS) $(IFLAGS) $(LFLAGS) $(OBJ) -ltbb -o sl

clean:
	rm -f list hash sl
	rm -f ./include/libssmem.a
	rm -f $(OBJ) main
