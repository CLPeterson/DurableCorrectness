CC=g++
CFLAGS=-I. 
DEPS = 
OBJ = visual.o

%.o: %.cc $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

visual: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
	
clean:
	rm -f visual $(OBJ)
