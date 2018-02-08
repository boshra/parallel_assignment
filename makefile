CC=mpicc
CFLAGS=
OBJ=blocking.o 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: bl nonbl coll

bl: blocking.o
	mpicc -o $@ $^ $(CFLAGS)

nonbl: nonblocking.o
	mpicc -o $@ $^ $(CFLAGS)

coll: collective.o
	mpicc -o $@ $^ $(CFLAGS)

test: mpimake
	bash test.sh

report:
	pdflatex report.tex

.PHONY: clean

clean:
	rm -f *.o *.txt
