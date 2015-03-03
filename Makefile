#You can use either a gcc or g++ compiler
#CC = gcc
CC = g++
EXECUTABLES = main
CFLAGS = -I.

OPTFLAG = -O2
DEBUGFLAG = -g

all: ${EXECUTABLES}

test: CFLAGS += $(OPTFLAG)
test: ${EXECUTABLES}
	for exec in ${EXECUTABLES}; do \
    		./$$exec ; \
	done

debug: CFLAGS += $(DEBUGFLAG)
debug: $(EXECUTABLES)
	for dbg in ${EXECUTABLES}; do \
		gdb ./$$dbg ; \
	done

main: main.cc ipsum.o
	$(CC) $(CFLAGS) -o main main.cc ipsum.o -lpthread


ipsum.o: ipsum.c
	$(CC) $(CFLAGS) -c ipsum.c 
clean:
	rm -f *.o ${EXECUTABLES} a.out
