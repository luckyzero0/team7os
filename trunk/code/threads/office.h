#ifndef OFFICE_H
#define OFFICE_H

#include "synch.h"

#define MAX_APP_CLERKS (10)
#define MAX_PIC_CLERKS (10)
#define TRUE (1)
#define FALSE (0)

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

// Total Number of Things in The Office
extern int totalCustomersInOffice;

// ApplicationClerk and PictureClerk States
extern ClerkStatus appClerkStatuses[];
extern ClerkStatus picClerkStatuses[];
extern Lock* appClerkLocks[];
extern Lock* picClerkLocks[];
extern Condition* appClerkCVs[];
extern Condition* picClerkCVs[];
extern int appClerkData[];
extern int picClerkData[];

extern void Office();

#endif
