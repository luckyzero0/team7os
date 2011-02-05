#ifndef OFFICE_H
#define OFFICE_H

#include "synch.h"
#include <bool.h>

#define MAX_APP_CLERKS (10)
#define MAX_PIC_CLERKS (10)
#define MAX_PASS_CLERKS (10)
#define MAX_CASH_CLERKS (10)
#define MAX_CUSTOMERS (100)


enum ClerkStatus {CLERK_BUSY, CLERK_AVAILABLE, CLERK_ON_BREAK, CLERK_INVALID};
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
extern Condition* privCashLineCV;
extern Condition* regCashLineCV;
extern int privCashLineLength;
extern int regCashLineLength;

// Total Number of Things in The Office
extern int totalCustomersInOffice;

// ApplicationClerk and PictureClerk States
extern ClerkStatus appClerkStatuses[];
extern ClerkStatus picClerkStatuses[];
extern Lock* appClerkLocks[];
extern Lock* picClerkLocks[];
extern Condition* appClerkCVs[];
extern Condition* picClerkCVs[];
extern int appClerkSSNs[];
extern int picClerkSSNs[];
extern int happyWithPhoto[];

extern ClerkStatus passClerkStatuses[];
extern Lock* passClerkLocks[];
extern Condition* passClerkCVs[];
extern int passClerkSSNs[];
extern int passPunish[];

extern ClerkStatus cashClerkStatuses[];
extern Lock* cashClerkLocks[];
extern Condition* cashClerkCVs[];
extern int cashClerkSSNs[];
extern int cashPunish[];

extern int appFiled[];
extern int picFiled[];
extern int passFiled[];

extern void Office();

#endif
