CC=mpicc
CFLAGS=
OBJ=blocking.o 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

mpimake: $(OBJ)
	mpicc -o $@ $^ $(CFLAGS)

test: mpimake
	mpirun -n 1  mpimake

report:
	pdflatex report.tex

.PHONY: clean

clean:
	rm -f *.o
