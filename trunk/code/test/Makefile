# use normal make for this Makefile
#
# Makefile for building user programs to run on top of Nachos
#
# Several things to be aware of:
#
#    Nachos assumes that the location of the program startup routine (the
# 	location the kernel jumps to when the program initially starts up)
#       is at location 0.  This means: start.o must be the first .o passed 
# 	to ld, in order for the routine "Start" to be loaded at location 0
#

# If you are running on SunOS 5.x machines, uncomment the next line
GCCDIR = /auto/home-scf-22/csci402/testgnu/decstation-ultrix/bin/
LDFLAGS = -T newscript -N
ASFLAGS = -mips1
CPPFLAGS = $(INCDIR)

# if you aren't cross-compiling:
# GCCDIR =
# LDFLAGS = -N -T 0
# ASFLAGS =
# CPPFLAGS = -P $(INCDIR)


CC = $(GCCDIR)gcc
AS = $(GCCDIR)as
LD = $(GCCDIR)ld
CPP = $(GCCDIR)cpp

INCDIR =-I../userprog -I../threads
CFLAGS = -G 0 -c $(INCDIR)

all: halt matmult sort testfiles

start.o: start.s ../userprog/syscall.h
	$(CPP) $(CPPFLAGS) start.s > strt.s
	$(AS) $(ASFLAGS) -o start.o strt.s
	rm strt.s

halt.o: halt.c
	$(CC) $(CFLAGS) -c halt.c
halt: halt.o start.o
	$(LD) $(LDFLAGS) start.o halt.o -o halt.coff
	../bin/coff2noff halt.coff halt

sort.o: sort.c
	$(CC) $(CFLAGS) -c sort.c utils.c
sort: sort.o start.o utils.o
	$(LD) $(LDFLAGS) start.o sort.o utils.o -o sort.coff
	../bin/coff2noff sort.coff sort

matmult.o: matmult.c
	$(CC) $(CFLAGS) -c matmult.c
matmult: matmult.o start.o
	$(LD) $(LDFLAGS) start.o matmult.o -o matmult.coff
	../bin/coff2noff matmult.coff matmult

testLocks.o: testLocks.c
	$(CC) $(CFLAGS) -c testLocks.c utils.c
testLocks: testLocks.o start.o utils.o
	$(LD) $(LDFLAGS) start.o testLocks.o utils.o -o testLocks.coff
	../bin/coff2noff testLocks.coff testLocks

exec_testLocks.o: exec_testLocks.c
	$(CC) $(CFLAGS) -c exec_testLocks.c utils.c
exec_testLocks: exec_testLocks.o start.o utils.o
	$(LD) $(LDFLAGS) start.o exec_testLocks.o utils.o -o exec_testLocks.coff
	../bin/coff2noff exec_testLocks.coff exec_testLocks

testfiles.o: testfiles.c
	$(CC) $(CFLAGS) -c testfiles.c
testfiles: testfiles.o start.o
	$(LD) $(LDFLAGS) start.o testfiles.o -o testfiles.coff
	../bin/coff2noff testfiles.coff testfiles


forkTest.o: forkTest.c
	$(CC) $(CFLAGS) -c forkTest.c utils.c
forkTest: forkTest.o start.o utils.o
	$(LD) $(LDFLAGS) start.o forkTest.o utils.o -o forkTest.coff
	../bin/coff2noff forkTest.coff forkTest

randTest.o: randTest.c
	$(CC) $(CFLAGS) -c randTest.c utils.c
randTest: randTest.o start.o utils.o
	$(LD) $(LDFLAGS) start.o randTest.o utils.o -o randTest.coff
	../bin/coff2noff randTest.coff randTest
	
	
execTest.o: execTest.c
	$(CC) $(CFLAGS) -c execTest.c utils.c
execTest: execTest.o start.o utils.o
	$(LD) $(LDFLAGS) start.o execTest.o utils.o -o execTest.coff
	../bin/coff2noff execTest.coff execTest

clean:
	rm -f *.o *.coff
