#ifndef OFFICE_H
#define OFFICE_H

#include "synch.h"

#define MAX_APP_CLERKS (10)
#define MAX_PIC_CLERKS (10)

enum ClerkStatus {CLERK_BUSY, CLERK_AVAILABLE, CLERK_ON_BREAK, CLERK_INVALID};

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

extern void Office();

#endif
