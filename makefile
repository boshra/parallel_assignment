CC=mpicc
CFLAGS=
SRCPATH=src/
REPPATH=report/

%.o: $(SRCPATH)%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: bl nonbl coll

bl: blocking.o
	mpicc -o $@ $^ $(CFLAGS)

nonbl: nonblocking.o
	mpicc -o $@ $^ $(CFLAGS)

coll: collective.o
	mpicc -o $@ $^ $(CFLAGS)

test: all
	bash test.sh

report.pdf:
	cd report && pdflatex $(REPPATH)report.tex

.PHONY: clean

clean:
	rm -f *.o *.txt *.pdf *.csv *.log *.aux
