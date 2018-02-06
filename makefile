CC=mpicc
CFLAGS=
OBJ=blocking.o 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

mpimake: $(OBJ)
	mpicc -o $@ $^ $(CFLAGS)

test: mpimake
	mpirun -n 1  mpimake

.PHONY: clean

clean:
	rm -f *.o
