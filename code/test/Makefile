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

all: halt forkArg exec_testLocks execTest forkTest sort testfiles matmult exec_matmult exec_sort mc_signal mc_wait forkSort testLocks testConditions testMonitors netOffice multiClientTest

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

forkTest.o: forkTest.c
	$(CC) $(CFLAGS) -c forkTest.c utils.c
forkTest: forkTest.o start.o utils.o
	$(LD) $(LDFLAGS) start.o forkTest.o utils.o -o forkTest.coff
	../bin/coff2noff forkTest.coff forkTest

testLocks.o: testLocks.c
	$(CC) $(CFLAGS) -c testLocks.c utils.c
testLocks: testLocks.o start.o utils.o
	$(LD) $(LDFLAGS) start.o testLocks.o utils.o -o testLocks.coff
	../bin/coff2noff testLocks.coff testLocks
	
forkSort.o: forkSort.c
	$(CC) $(CFLAGS) -c forkSort.c utils.c
forkSort: forkSort.o start.o utils.o
	$(LD) $(LDFLAGS) start.o forkSort.o utils.o -o forkSort.coff
	../bin/coff2noff forkSort.coff forkSort
	
mc_signal.o: mc_signal.c
	$(CC) $(CFLAGS) -c mc_signal.c utils.c
mc_signal: mc_signal.o start.o utils.o
	$(LD) $(LDFLAGS) start.o mc_signal.o utils.o -o mc_signal.coff
	../bin/coff2noff mc_signal.coff mc_signal
	
mc_wait.o: mc_wait.c
	$(CC) $(CFLAGS) -c mc_wait.c utils.c
mc_wait: mc_wait.o start.o utils.o
	$(LD) $(LDFLAGS) start.o mc_wait.o utils.o -o mc_wait.coff
	../bin/coff2noff mc_wait.coff mc_wait

exec_testLocks.o: exec_testLocks.c
	$(CC) $(CFLAGS) -c exec_testLocks.c utils.c
exec_testLocks: exec_testLocks.o start.o utils.o
	$(LD) $(LDFLAGS) start.o exec_testLocks.o utils.o -o exec_testLocks.coff
	../bin/coff2noff exec_testLocks.coff exec_testLocks
	
exec_sort.o: exec_sort.c
	$(CC) $(CFLAGS) -c exec_sort.c utils.c
exec_sort: exec_sort.o start.o utils.o
	$(LD) $(LDFLAGS) start.o exec_sort.o utils.o -o exec_sort.coff
	../bin/coff2noff exec_sort.coff exec_sort

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
	
matmult.o: matmult.c
	$(CC) $(CFLAGS) -c matmult.c utils.c
matmult: matmult.o start.o utils.o
	$(LD) $(LDFLAGS) start.o matmult.o utils.o -o matmult.coff
	../bin/coff2noff matmult.coff matmult

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
	
forkArg.o: forkArg.c
	$(CC) $(CFLAGS) -c forkArg.c utils.c
forkArg: forkArg.o start.o utils.o
	$(LD) $(LDFLAGS) start.o forkArg.o utils.o -o forkArg.coff
	../bin/coff2noff forkArg.coff forkArg

office.o: office.c
	$(CC) $(CFLAGS) -c office.c utils.c
	
clerks.o: clerks.c
	$(CC) $(CFLAGS) -c clerks.c utils.c
	
netAppClerk.o: netAppClerk.c utils.h officeData.h
	$(CC) $(CFLAGS) -c netAppClerk.c utils.c
netAppClerk: netAppClerk.o start.o utils.o
	$(LD) $(LDFLAGS) start.o netAppClerk.o utils.o -o netAppClerk.coff
	../bin/coff2noff netAppClerk.coff netAppClerk
	
netPicClerk.o: netPicClerk.c utils.h officeData.h
	$(CC) $(CFLAGS) -c netPicClerk.c utils.c
netPicClerk: netPicClerk.o start.o utils.o
	$(LD) $(LDFLAGS) start.o netPicClerk.o utils.o -o netPicClerk.coff
	../bin/coff2noff netPicClerk.coff netPicClerk
	
netPassClerk.o: netPassClerk.c utils.h officeData.h
	$(CC) $(CFLAGS) -c netPassClerk.c utils.c
netPassClerk: netPassClerk.o start.o utils.o
	$(LD) $(LDFLAGS) start.o netPassClerk.o utils.o -o netPassClerk.coff
	../bin/coff2noff netPassClerk.coff netPassClerk
	
netCashClerk.o: netCashClerk.c utils.h officeData.h
	$(CC) $(CFLAGS) -c netCashClerk.c utils.c
netCashClerk: netCashClerk.o start.o utils.o
	$(LD) $(LDFLAGS) start.o netCashClerk.o utils.o -o netCashClerk.coff
	../bin/coff2noff netCashClerk.coff netCashClerk
	
netManager.o: netManager.c utils.h officeData.h
	$(CC) $(CFLAGS) -c netManager.c utils.c
netManager: netManager.o start.o utils.o
	$(LD) $(LDFLAGS) start.o netManager.o utils.o -o netManager.coff
	../bin/coff2noff netManager.coff netManager
	
customer.o: customer.c
	$(CC) $(CFLAGS) -c customer.c utils.c
	
senator.o: senator.c
	$(CC) $(CFLAGS) -c senator.c utils.c
	
manager.o: manager.c
	$(CC) $(CFLAGS) -c manager.c utils.c
	
office: office.o clerks.o customer.o senator.o manager.o start.o utils.o
	$(LD) $(LDFLAGS) start.o office.o clerks.o customer.o senator.o manager.o utils.o -o office.coff
	../bin/coff2noff office.coff office
	
exec_office.o: exec_office.c
	$(CC) $(CFLAGS) -c exec_office.c utils.c
exec_office: exec_office.o start.o utils.o
	$(LD) $(LDFLAGS) start.o exec_office.o utils.o -o exec_office.coff
	../bin/coff2noff exec_office.coff exec_office
	
exec_matmult.o: exec_matmult.c
	$(CC) $(CFLAGS) -c exec_matmult.c utils.c
exec_matmult: exec_matmult.o start.o utils.o
	$(LD) $(LDFLAGS) start.o exec_matmult.o utils.o -o exec_matmult.coff
	../bin/coff2noff exec_matmult.coff exec_matmult
	
exec_forkTest.o: exec_forkTest.c
	$(CC) $(CFLAGS) -c exec_forkTest.c utils.c
exec_forkTest: exec_forkTest.o start.o utils.o
	$(LD) $(LDFLAGS) start.o exec_forkTest.o utils.o -o exec_forkTest.coff
	../bin/coff2noff exec_forkTest.coff exec_forkTest


testConditions.o: testConditions.c
	$(CC) $(CFLAGS) -c testConditions.c utils.c
testConditions: testConditions.o start.o utils.o
	$(LD) $(LDFLAGS) start.o testConditions.o utils.o -o testConditions.coff
	../bin/coff2noff testConditions.coff testConditions


officeMain.o: officeMain.c
	$(CC) $(CFLAGS) -c officeMain.c utils.c 
officeMain: officeMain.o start.o utils.o office.o manager.o customer.o clerks.o senator.o
	$(LD) $(LDFLAGS) start.o officeMain.o utils.o office.o manager.o customer.o clerks.o senator.o -o officeMain.coff
	../bin/coff2noff officeMain.coff officeMain

officeMainDEBUG.o: officeMainDEBUG.c
	$(CC) $(CFLAGS) -c officeMainDEBUG.c utils.c 
officeMainDEBUG: officeMainDEBUG.o start.o utils.o office.o manager.o customer.o clerks.o senator.o
	$(LD) $(LDFLAGS) start.o officeMainDEBUG.o utils.o office.o manager.o customer.o clerks.o senator.o -o officeMainDEBUG.coff
	../bin/coff2noff officeMainDEBUG.coff officeMainDEBUG


customerLineTest.o: customerLineTest.c
	$(CC) $(CFLAGS) -c customerLineTest.c utils.c 
customerLineTest: customerLineTest.o start.o utils.o office.o manager.o customer.o clerks.o senator.o
	$(LD) $(LDFLAGS) start.o customerLineTest.o utils.o office.o manager.o customer.o clerks.o senator.o -o customerLineTest.coff
	../bin/coff2noff customerLineTest.coff customerLineTest

managerTest.o: managerTest.c
	$(CC) $(CFLAGS) -c managerTest.c utils.c 
managerTest: managerTest.o start.o utils.o office.o manager.o customer.o clerks.o senator.o
	$(LD) $(LDFLAGS) start.o managerTest.o utils.o office.o manager.o customer.o clerks.o senator.o -o managerTest.coff
	../bin/coff2noff managerTest.coff managerTest

customerCashierTest.o: customerCashierTest.c
	$(CC) $(CFLAGS) -c customerCashierTest.c utils.c 
customerCashierTest: customerCashierTest.o start.o utils.o office.o manager.o customer.o clerks.o senator.o
	$(LD) $(LDFLAGS) start.o customerCashierTest.o utils.o office.o manager.o customer.o clerks.o senator.o -o customerCashierTest.coff
	../bin/coff2noff customerCashierTest.coff customerCashierTest


clerkBreakTest.o: clerkBreakTest.c
	$(CC) $(CFLAGS) -c clerkBreakTest.c utils.c 
clerkBreakTest: clerkBreakTest.o start.o utils.o office.o manager.o customer.o clerks.o senator.o
	$(LD) $(LDFLAGS) start.o clerkBreakTest.o utils.o office.o manager.o customer.o clerks.o senator.o -o clerkBreakTest.coff
	../bin/coff2noff clerkBreakTest.coff clerkBreakTest

managerClerkTest.o: managerClerkTest.c
	$(CC) $(CFLAGS) -c managerClerkTest.c utils.c 
managerClerkTest: managerClerkTest.o start.o utils.o office.o manager.o customer.o clerks.o senator.o
	$(LD) $(LDFLAGS) start.o managerClerkTest.o utils.o office.o manager.o customer.o clerks.o senator.o -o managerClerkTest.coff
	../bin/coff2noff managerClerkTest.coff managerClerkTest
	
senatorTest.o: senatorTest.c
	$(CC) $(CFLAGS) -c senatorTest.c utils.c 
senatorTest: senatorTest.o start.o utils.o office.o manager.o customer.o clerks.o senator.o
	$(LD) $(LDFLAGS) start.o senatorTest.o utils.o office.o manager.o customer.o clerks.o senator.o -o senatorTest.coff
	../bin/coff2noff senatorTest.coff senatorTest

NTLockCVs.o: NTLockCVs.c
	$(CC) $(CFLAGS) -c NTLockCVs.c utils.c
NTLockCVs: NTLockCVs.o start.o utils.o
	$(LD) $(LDFLAGS) start.o NTLockCVs.o utils.o -o NTLockCVs.coff
	../bin/coff2noff NTLockCVs.coff NTLockCVs

testMonitors.o: testMonitors.c
	$(CC) $(CFLAGS) -c testMonitors.c utils.c
testMonitors: testMonitors.o start.o utils.o
	$(LD) $(LDFLAGS) start.o testMonitors.o utils.o -o testMonitors.coff
	../bin/coff2noff testMonitors.coff testMonitors

multiClientTest.o: multiClientTest.c
	$(CC) $(CFLAGS) -c multiClientTest.c utils.c
multiClientTest: multiClientTest.o start.o utils.o
	$(LD) $(LDFLAGS) start.o multiClientTest.o utils.o -o multiClientTest.coff
	../bin/coff2noff multiClientTest.coff multiClientTest

netCustomer.o: netCustomer.c
	$(CC) $(CFLAGS) -c netCustomer.c utils.c
netCustomer: netCustomer.o start.o utils.o
	$(LD) $(LDFLAGS) start.o netCustomer.o utils.o -o netCustomer.coff
	../bin/coff2noff netCustomer.coff netCustomer

netSenator.o: netSenator.c
	$(CC) $(CFLAGS) -c netSenator.c utils.c
netSenator: netSenator.o start.o utils.o
	$(LD) $(LDFLAGS) start.o netSenator.o utils.o -o netSenator.coff
	../bin/coff2noff netSenator.coff netSenator
	
net10Cust.o: net10Cust.c
	$(CC) $(CFLAGS) -c net10Cust.c
net10Cust: net10Cust.o start.o
	$(LD) $(LDFLAGS) start.o net10Cust.o -o net10Cust.coff
	../bin/coff2noff net10Cust.coff net10Cust
	
netAPPM.o: netAPPM.c
	$(CC) $(CFLAGS) -c netAPPM.c
netAPPM: netAPPM.o start.o
	$(LD) $(LDFLAGS) start.o netAPPM.o -o netAPPM.coff
	../bin/coff2noff netAPPM.coff netAPPM

netCashSen.o: netCashSen.c
	$(CC) $(CFLAGS) -c netCashSen.c
netCashSen: netCashSen.o start.o
	$(LD) $(LDFLAGS) start.o netCashSen.o -o netCashSen.coff
	../bin/coff2noff netCashSen.coff netCashSen
	
netOffice: netCustomer netSenator netManager netAppClerk netPicClerk netPassClerk netCashClerk net10Cust netAPPM netCashSen

clean:
	rm -f *.o *.coff
