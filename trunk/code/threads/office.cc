#include "office.h"
#include "thread.h"
#include <stdio.h>

// ApplicationClerk and PictureClerk lines
Lock* appPicLineLock = new Lock("appPicLineLock");
Condition* regAppLineCV = new Condition("regAppLineCV");
Condition* privAppLineCV = new Condition("privAppLinCV");
Condition* regPicLineCV = new Condition("regPicLineCV");
Condition* privPicLineCV = new Condition("privPicLineCV");
int regAppLineLength = 0;
int privAppLineLength = 0;
int regPicLineLength = 0;
int privPicLineLength = 0;


// Total Number of Things in The Office
int totalCustomersInOffice = 0;

// ApplicationClerk and PictureClerk States
ClerkStatus appClerkStatuses[MAX_APP_CLERKS];
ClerkStatus picClerkStatuses[MAX_PIC_CLERKS];
Lock* appClerkLocks[MAX_APP_CLERKS];
Lock* picClerkLocks[MAX_PIC_CLERKS];
Condition* appClerkCVs[MAX_APP_CLERKS];
Condition* picClerkCVs[MAX_PIC_CLERKS];

extern void CustomerRun(int);
extern void AppClerkRun(int);
extern void PicClerkRun(int);

void initializeArrays(int, int);

void Office() {
  
  int numAppClerks = 3; // WE WANT TO GET THESE FROM USER INPUT AND VALIDATE
  int numPicClerks = 3; //
  int numCustomers = 1;
  srand(time(NULL));
  
  initializeArrays(numAppClerks, numPicClerks);

  // Fork the application clerks
  
  for (int i = 0; i < numAppClerks; i++) {
    char* name = new char[20];
    snprintf(name, 20, "AppClerk%d", i);
    Thread* t = new Thread(name); // HACK
    t->Fork((VoidFunctionPtr)AppClerkRun, i);
    printf("Forked %s\n", name);
  }

  // Fork the picture clerks
  for (int i = 0; i < numPicClerks; i++) {
    char* name = new char[20];
    snprintf(name, 20, "PicClerk%d", i);
    Thread* t = new Thread(name);
    t->Fork((VoidFunctionPtr)PicClerkRun, i);
    printf("Forked %s\n", name);
  }

  // Fork the customers
  for (int i = 0; i < numCustomers; i++) {
    char* name = new char[20];
    snprintf(name, 20, "Customer%d", i);
    Thread* t = new Thread(name); // HACK, need to get correct naming / numbering
    t->Fork((VoidFunctionPtr)CustomerRun, i);
    printf("Forked %s\n", name);
  }
}

void initializeArrays(int numAppClerks, int numPicClerks) {

  for (int i = 0; i < MAX_APP_CLERKS; i++) {
    if (i < numAppClerks) {
      appClerkStatuses[i] = CLERK_AVAILABLE;
      char* name = new char[20];
      snprintf(name, 20, "AppClerkLock%d", i);
      appClerkLocks[i] = new Lock(name);
      name = new char[20];
      snprintf(name, 20, "AppClerkCV%d", i);
      appClerkCVs[i] = new Condition(name);
    }
    else {
      appClerkStatuses[i] = CLERK_INVALID;
      appClerkLocks[i] = NULL;
      appClerkCVs[i] = NULL;
    }
  }
  
  for (int i = 0; i < MAX_PIC_CLERKS; i++) {
    if (i < numPicClerks) {
      picClerkStatuses[i] = CLERK_AVAILABLE;
      char* name = new char[20];
      snprintf(name, 20, "PicClerkLock%d", i);
      picClerkLocks[i] = new Lock(name);
      name = new char[20];
      snprintf(name, 20, "PicClerkCV%d", i);
      picClerkCVs[i] = new Condition(name);
    }
    else {
      picClerkStatuses[i] = CLERK_INVALID;
      picClerkLocks[i] = NULL;
      picClerkCVs[i] = NULL;
    }
  }
}
