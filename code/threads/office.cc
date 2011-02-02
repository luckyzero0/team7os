#include "office.h"
#include "thread.h"

// ApplicationClerk and PictureClerk lines
Lock* appPicLineLock = new Lock("appPicLineLock");
Condition* regAppLineCV = new Condition("regAppLineCV");
Condition* privAppLineCV = new Condition("privAppLinCV");
Condition* regPicLineCV = new Condition("regPicLineCV");
Condition* privAppLineCV = new Condition("privAppLineCV");
int regAppLineLength = 0;
int privAppLineLength = 0;
int regPicLineLength = 0;
int privPicLineLength = 0;

// Total Number of Things in The Office
int totalCustomersInOffice = 0;

// ApplicationClerk and PictureClerk States
ClerkStatus appClerkStatuses[MAX_APP_CLERKS];
ClerkStatus picClerkStatuses[MAX_PIC_CLERKS];

void Office() {
  
  initializeArrays();
  
  int numAppClerks = 3; // WE WANT TO GET THESE FROM USER INPUT AND VALIDATE
  int numPicClerks = 3; //
  int numCustomers = 1;
  
  initializeArrays(numAppClerks, numPicClerks);

  // Fork the application clerks
  for (int i = 0; i < numAppClerks; i++) {
    Thread* t = new Thread("AppClerk1"); // HACK
    t->Fork((VoidFunctionPtr)AppClerkRun, i);
  }

  for (int i = 0; i < numPicClerks; i++) {
    Thread* t = new Thread("PicClerk1");
    t->Fork((VoidFunctionPtr)PicClerkRun, i);
  }

  // Fork the customers
  for (int i = 0; i < numCustomers; i++) {
    Thread* t = new Thread("Customer1"); // HACK, need to get correct naming / numbering
    t->Fork((VoidFunctionPtr)CustomerRun, i);
  }
}

void initializeArrays(int numAppClerks, int numPicClerks) {

  for (int i = 0; i < MAX_APP_CLERKS; i++) {
    if (i < numAppClerks) {
      appClerkStatuses[i] = CLERK_AVAILABLE;
    }
    else {
      appClerkStatuses[i] = CLERK_INVALID;
    }
  }
  
  for (int i = 0; i < MAX_PIC_CLERKS; i++) {
    if (i < numPicClerks) {
      picClerkStatuses[i] = CLERK_AVAILABLE;
    }
    else {
      picClerkStatuses[i] = CLERK_INVALID;
    }
  }
}
