CC=g++
CFLAGS=-I. -ggdb
DEPS = 
OBJ = scriptPetra.o scriptRomulus.o scriptOneFile.o scriptPMDK.o

all: scriptPetra scriptRomulus scriptOneFile scriptPMDK

scriptPetra.o: scriptPetra.cc $(DEPS)
	$(CC) -c scriptPetra.cc -o scriptPetra.o $(CFLAGS)

scriptPetra: scriptPetra.o
	$(CC) -o scriptPetra scriptPetra.o $(CFLAGS)

scriptRomulus.o: scriptRomulus.cc $(DEPS)
	$(CC) -c scriptRomulus.cc -o scriptRomulus.o $(CFLAGS)

scriptRomulus: scriptRomulus.o
	$(CC) -o scriptRomulus scriptRomulus.o $(CFLAGS)

scriptOneFile.o: scriptOneFile.cc $(DEPS)
	$(CC) -c scriptOneFile.cc -o scriptOneFile.o $(CFLAGS)

scriptOneFile: scriptOneFile.o
	$(CC) -o scriptOneFile scriptOneFile.o $(CFLAGS)

scriptPMDK.o: scriptPMDK.cc $(DEPS)
	$(CC) -c scriptPMDK.cc -o scriptPMDK.o $(CFLAGS)

scriptPMDK: scriptPMDK.o
	$(CC) -o scriptPMDK scriptPMDK.o $(CFLAGS)
	
clean:
	rm -f scriptPetra scriptRomulus scriptOneFile scriptPMDK $(OBJ)

