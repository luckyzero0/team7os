#ifndef OFFICE_H
#define OFFICE_H

#include "synch.h"
#include <bool.h>

#define MAX_APP_CLERKS (10)
#define MAX_PIC_CLERKS (10)
#define MAX_PASS_CLERKS (10)
#define MAX_CASH_CLERKS (10)
#define MAX_CUSTOMERS (100)

extern int TESTING;
extern int FINISHED_FORKING;

enum ClerkStatus {CLERK_BUSY, CLERK_AVAILABLE, CLERK_ON_BREAK, CLERK_INVALID, CLERK_COMING_BACK};
enum ClerkType {APPLICATION, PICTURE, PASSPORT, CASHIER};
// ApplicationClerk and PictureClerk lines
extern Lock* appPicLineLock;

extern Condition* regAppLineCV;
extern Condition* privAppLineCV;

extern Condition* regPicLineCV;
extern Condition* privPicLineCV;

extern int regAppLineLength;
extern int privAppLineLength;

extern int regPicLineLength;
extern int privPicLineLength;

extern Lock* passLineLock;
extern Condition* privPassLineCV;
extern Condition* regPassLineCV;
extern int privPassLineLength;
extern int regPassLineLength;

extern Lock* cashLineLock;
// extern Condition* privCashLineCV; -- THIS CAN NEVER HAPPEN
extern Condition* regCashLineCV;
// extern int privCashLineLength; -- THIS CAN NEVER HAPPEN
extern int regCashLineLength;

// Senators in office and waiting room
extern Lock* senatorWaitingRoomLock;
extern Condition* senatorWaitingRoomCV;
extern int senatorsInWaitingRoom;

extern Lock* senatorOfficeLock;
extern int senatorsInOffice;

// Customers in office and waiting room
extern Lock* customerWaitingRoomLock;
extern Condition* customerWaitingRoomCV;
extern int customersInWaitingRoom;

extern Lock* customerOfficeLock;
extern int customersInOffice;

extern int numCustomers;
extern int numAppClerks;
extern int numPicClerks;
extern int numPassClerks;
extern int numCashClerks;
extern int numSenators;

// ApplicationClerk and PictureClerk States
extern ClerkStatus appClerkStatuses[];
extern ClerkStatus picClerkStatuses[];
extern Lock* appClerkLocks[];
extern Lock* picClerkLocks[];
extern Condition* appClerkCVs[];
extern Condition* picClerkCVs[];
extern int appClerkSSNs[];
extern int appClerkMoney[];
extern int appClerkBribed[];
extern int picClerkSSNs[];
extern int picClerkMoney[];
extern int picClerkBribed[];
extern int happyWithPhoto[];

extern ClerkStatus passClerkStatuses[];
extern Lock* passClerkLocks[];
extern Condition* passClerkCVs[];
extern int passClerkSSNs[];
extern int passClerkMoney[];
extern int passClerkBribed[];
extern int passPunish[];

extern ClerkStatus cashClerkStatuses[];
extern Lock* cashClerkLocks[];
extern Condition* cashClerkCVs[];
extern int cashClerkSSNs[];
extern int cashClerkMoney[];
extern int cashPunish[];

extern int appFiled[];
extern int picFiled[];
extern int passFiled[];
extern int cashFiled[];

extern void Office();
extern void tprintf(char*, ...);
#endif
