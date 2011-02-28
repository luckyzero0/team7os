#ifndef OFFICE_H
#define OFFICE_H

#include "syscall.h"
#include "utils.h"

#define TRUE 1
#define FALSE 0

#define NULL 0

#define MAX_APP_CLERKS (3)
#define MAX_PIC_CLERKS (3)
#define MAX_PASS_CLERKS (3)
#define MAX_CASH_CLERKS (3)
#define MAX_CUSTOMERS (23)

#define CLERK_BUSY 0
#define CLERK_AVAILABLE 1
#define CLERK_ON_BREAK 2
#define CLERK_INVALID 3
#define CLERK_COMING_BACK 4

#define APPLICATION 0
#define PICTURE 1
#define PASSPORT 2
#define CASHIER 3

typedef int bool;
typedef int ClerkStatus;
typedef int ClerkType;

extern int TESTING;
extern int FINISHED_FORKING;
extern int needManager;

/*enum ClerkStatus {CLERK_BUSY, CLERK_AVAILABLE, CLERK_ON_BREAK, CLERK_INVALID, CLERK_COMING_BACK};
enum ClerkType {APPLICATION, PICTURE, PASSPORT, CASHIER};*/
/* ApplicationClerk and PictureClerk lines*/
extern LockID appPicLineLock;

extern ConditionID regAppLineCV;
extern ConditionID privAppLineCV;

extern ConditionID regPicLineCV;
extern ConditionID privPicLineCV;

extern int regAppLineLength;
extern int privAppLineLength;

extern int regPicLineLength;
extern int privPicLineLength;

extern LockID passLineLock;
extern ConditionID privPassLineCV;
extern ConditionID regPassLineCV;
extern int privPassLineLength;
extern int regPassLineLength;

extern LockID cashLineLock;
/* extern ConditionID privCashLineCV; -- THIS CAN NEVER HAPPEN*/
extern ConditionID regCashLineCV;
/* extern int privCashLineLength; -- THIS CAN NEVER HAPPEN*/
extern int regCashLineLength;

/* Senators in office and waiting room*/
extern LockID senatorWaitingRoomLock;
extern ConditionID senatorWaitingRoomCV;
extern int senatorsInWaitingRoom;

extern LockID senatorOfficeLock;
extern int senatorsInOffice;

/* Customers in office and waiting room*/
extern LockID customerWaitingRoomLock;
extern ConditionID customerWaitingRoomCV;
extern int customersInWaitingRoom;

extern LockID customerOfficeLock;
extern int customersInOffice;

extern LockID entryLock;

extern LockID appClerkUIDLock;
extern LockID picClerkUIDLock;
extern LockID passClerkUIDLock;
extern LockID cashClerkUIDLock;
extern LockID customerSenatorUIDLock;

extern int appClerkUID;
extern int picClerkUID;
extern int passClerkUID;
extern int cashClerkUID;
extern int customerSenatorUID;


extern int numCustomers;
extern int numAppClerks;
extern int numPicClerks;
extern int numPassClerks;
extern int numCashClerks;
extern int numSenators;

/* ApplicationClerk and PictureClerk States*/
extern ClerkStatus appClerkStatuses[];
extern ClerkStatus picClerkStatuses[];
extern LockID appClerkLocks[];
extern LockID picClerkLocks[];
extern ConditionID appClerkCVs[];
extern ConditionID picClerkCVs[];
extern int appClerkSSNs[];
extern int appClerkMoney[];
extern int appClerkBribed[];
extern int picClerkSSNs[];
extern int picClerkMoney[];
extern int picClerkBribed[];
extern int happyWithPhoto[];

extern ClerkStatus passClerkStatuses[];
extern LockID passClerkLocks[];
extern ConditionID passClerkCVs[];
extern int passClerkSSNs[];
extern int passClerkMoney[];
extern int passClerkBribed[];
extern int passPunish[];

extern ClerkStatus cashClerkStatuses[];
extern LockID cashClerkLocks[];
extern ConditionID cashClerkCVs[];
extern int cashClerkSSNs[];
extern int cashClerkMoney[];
extern int cashPunish[];

extern int appFiled[];
extern int picFiled[];
extern int passFiled[];
extern int cashFiled[];

extern void Office();
extern void tprintf(char*, int, int, int, char*, char*);
#endif
