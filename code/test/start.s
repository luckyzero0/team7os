
/* Start.s 
 *	Assembly language assist for user programs running on top of Nachos.
 *
 *	Since we don't want to pull in the entire C library, we define
 *	what we need for a user program here, namely Start and the system
 *	calls.
 */

#define IN_ASM
#include "syscall.h"

        .text   
        .align  2

/* -------------------------------------------------------------
 * __start
 *	Initialize running a C program, by calling "main". 
 *
 * 	NOTE: This has to be first, so that it gets loaded at location 0.
 *	The Nachos kernel always starts a program by jumping to location 0.
 * -------------------------------------------------------------
 */

	.globl __start
	.ent	__start
__start:
	jal	main
	move	$4,$0		
	jal	Exit	 /* if we return from main, exit(0) */
	.end __start

/* -------------------------------------------------------------
 * System call stubs:
 *	Assembly language assist to make system calls to the Nachos kernel.
 *	There is one stub per system call, that places the code for the
 *	system call into register r2, and leaves the arguments to the
 *	system call alone (in other words, arg1 is in r4, arg2 is 
 *	in r5, arg3 is in r6, arg4 is in r7)
 *
 * 	The return value is in r2. This follows the standard C calling
 * 	convention on the MIPS.
 * -------------------------------------------------------------
 */

	.globl Halt
	.ent	Halt
Halt:
	addiu $2,$0,SC_Halt
	syscall
	j	$31
	.end Halt

	.globl Exit
	.ent	Exit
Exit:
	addiu $2,$0,SC_Exit
	syscall
	j	$31
	.end Exit

	.globl Exec
	.ent	Exec
Exec:
	addiu $2,$0,SC_Exec
	syscall
	j	$31
	.end Exec

	.globl Join
	.ent	Join
Join:
	addiu $2,$0,SC_Join
	syscall
	j	$31
	.end Join

	.globl Create
	.ent	Create
Create:
	addiu $2,$0,SC_Create
	syscall
	j	$31
	.end Create

	.globl Open
	.ent	Open
Open:
	addiu $2,$0,SC_Open
	syscall
	j	$31
	.end Open

	.globl Read
	.ent	Read
Read:
	addiu $2,$0,SC_Read
	syscall
	j	$31
	.end Read

	.globl Write
	.ent	Write
Write:
	addiu $2,$0,SC_Write
	syscall
	j	$31
	.end Write

	.globl Close
	.ent	Close
Close:
	addiu $2,$0,SC_Close
	syscall
	j	$31
	.end Close

	.globl Fork
	.ent	Fork
Fork:
	addiu $2,$0,SC_Fork
	syscall
	j	$31
	.end Fork

	.globl Yield
	.ent	Yield
Yield:
	addiu $2,$0,SC_Yield
	syscall
	j	$31
	.end Yield

/* Project2 Additions */
	
	.globl CreateLock
	.ent	CreateLock
CreateLock:
	addiu $2,$0,SC_CreateLock
	syscall
	j	$31
	.end CreateLock

	.globl DestroyLock
	.ent	DestroyLock
DestroyLock:
	addiu $2,$0,SC_DestroyLock
	syscall
	j	$31
	.end DestroyLock
	
	.globl CreateCondition
	.ent	CreateCondition
CreateCondition:
	addiu $2,$0,SC_CreateCondition
	syscall
	j	$31
	.end CreateCondition
	
	.globl DestroyCondition
	.ent	DestroyCondition
DestroyCondition:
	addiu $2,$0,SC_DestroyCondition
	syscall
	j	$31
	.end DestroyCondition

	.globl Acquire
	.ent	Acquire
Acquire:
	addiu $2,$0,SC_Acquire
	syscall
	j	$31
	.end Acquire

	.globl Release
	.ent	Release
Release:
	addiu $2,$0,SC_Release
	syscall
	j	$31
	.end Release

	.globl Signal
	.ent	Signal
Signal:
	addiu $2,$0,SC_Signal
	syscall
	j	$31
	.end Signal

	.globl Wait
	.ent	Wait
Wait:
	addiu $2,$0,SC_Wait
	syscall
	j	$31
	.end Wait
	
	.globl Broadcast
	.ent	Broadcast
Broadcast:
	addiu $2,$0,SC_Broadcast
	syscall
	j	$31
	.end Broadcast
	
	.globl Rand
	.ent	Rand
Rand:
	addiu $2,$0,SC_Rand
	syscall
	j	$31
	.end Rand

	.globl ForkWithArg
	.ent ForkWithArg
ForkWithArg:
	addiu $2,$0,SC_ForkWithArg
	syscall
	j	$31
	.end ForkWithArg

	.globl GetForkArg
	.ent GetForkArg
GetForkArg:
	addiu $2,$0,SC_GetForkArg
	syscall
	j	$31
	.end GetForkArg

	.globl GetThreadID
	.ent GetThreadID
GetThreadID:
	addiu $2,$0,SC_GetThreadID
	syscall
	j	$31
	.end GetThreadID
	
/*Project 3 additions*/

	.globl SetMonitor
	.ent SetMonitor
SetMonitor:
	addiu $2,$0,SC_SetMonitor
	syscall
	j	$31
	.end SetMonitor
	
	.globl GetMonitor
	.ent GetMonitor
GetMonitor:
	addiu $2,$0,SC_GetMonitor
	syscall
	j	$31
	.end GetMonitor

	.globl CreateMonitor
	.ent CreateMonitor
CreateMonitor:
	addiu $2,$0,SC_CreateMonitor
	syscall
	j	$31
	.end CreateMonitor

.globl CreateMonitorArray
	.ent CreateMonitorArray
CreateMonitorArray:
	addiu $2,$0,SC_CreateMonitorArray
	syscall
	j	$31
	.end CreateMonitorArray

.globl GetMonitorArrayValue
	.ent GetMonitorArrayValue
GetMonitorArrayValue:
	addiu $2,$0,SC_GetMonitorArrayValue
	syscall
	j	$31
	.end GetMonitorArrayValue

.globl SetMonitorArrayValue
	.ent SetMonitorArrayValue
SetMonitorArrayValue:
	addiu $2,$0,SC_SetMonitorArrayValue
	syscall
	j	$31
	.end SetMonitorArrayValue

.globl DestroyMonitorArray
	.ent DestroyMonitorArray
DestroyMonitorArray:
	addiu $2,$0,SC_DestroyMonitorArray
	syscall
	j	$31
	.end DestroyMonitorArray

	.globl USleep
	.ent USleep
USleep:
	addiu $2,$0,SC_USleep
	syscall
	j	$31
	.end USleep


	.globl TimedSetMonitorArrayValue
	.ent TimedSetMonitorArrayValue
TimedSetMonitorArrayValue:
	addiu $2,$0,SC_TimedSetMonitorArrayValue
	syscall
	j	$31
	.end TimedSetMonitorArrayValue


/* dummy function to keep gcc happy */
        .globl  __main
        .ent    __main
__main:
        j       $31
        .end    __main


