ROOT = ../..

include $(ROOT)/common/Makefile.common

BINS = $(BINDIR)/wfrbt
PROF = $(ROOT)/src

.PHONY:    all clean

all: wfrbt.o wfrbt


VERSION = TCMALLOC_NO_UPDATE

CPPC = g++ -std=gnu++0x

ifeq "$(VERSION)" "MALLOC_UPDATE"
CFLAGS += -lpthread -lrt -DUPDATEVAL
else
ifeq "$(VERSION)" "TCMALLOC_UPDATE"
CFLAGS += -lpthread -lrt /usr/lib64/libtcmalloc.so.4 -DUPDATEVAL
else
ifeq "$(VERSION)" "JEMALLOC_NO_UPDATE"
CFLAGS += -pthread -lrt /home/naravind/research/jemalloc-x64/lib/libjemalloc.so.1
else
CFLAGS += -pthread -lrt
endif
endif
endif

INC = ../../../libnvram/include #../../../libatomic_ops/src/atomic_ops/sysdeps
OBJ = ../../../libnvram/epoch.o ../../../libnvram/link-cache.o ../../../libnvram/active-page-table.o

measurements.o:
	$(CC) $(CFLAGS) -c -o $(BUILDIR)/measurements.o $(PROF)/measurements.c -I$(INC)

wfrbt.o: measurements.o wfrbt.h wfrbt.cpp
	$(CPPC) $(CFLAGS) -c -o $(BUILDIR)/wfrbt.o wfrbt.cpp -I$(INC) 

wfrbt: measurements.o wfrbt.o
	$(CPPC) $(CFLAGS) $(OBJ) $(BUILDIR)/measurements.o $(BUILDIR)/wfrbt.o -o $(BINS) $(LDFLAGS) -I$(INC)


clean:
	rm -f $(BINS)
	rm -f $(BUILDIR)/wfrbt.o
