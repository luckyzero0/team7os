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

Lock* passLineLock = new Lock("passLineLock");
Condition* regPassLineCV = new Condition("regPassLineCV");
Condition* privPassLineCV = new Condition("privPassLineCV");
int regPassLineLength = 0;
int privPassLineLength = 0;

Lock* cashLineLock = new Lock("cashLineLock");
Condition* regCashLineCV = new Condition("regCashLineCV");
Condition* privCashLineCV = new Condition("privCashLineCV");
int regCashLineLength = 0;
int privCashLineLength = 0;

// Total Number of Things in The Office
int totalCustomersInOffice = 0;

// ApplicationClerk and PictureClerk States
ClerkStatus appClerkStatuses[MAX_APP_CLERKS];
ClerkStatus picClerkStatuses[MAX_PIC_CLERKS];
Lock* appClerkLocks[MAX_APP_CLERKS];
Lock* picClerkLocks[MAX_PIC_CLERKS];
Condition* appClerkCVs[MAX_APP_CLERKS];
Condition* picClerkCVs[MAX_PIC_CLERKS];
int appClerkSSNs[MAX_APP_CLERKS];
int picClerkSSNs[MAX_PIC_CLERKS];
int happyWithPhoto[MAX_PIC_CLERKS];

ClerkStatus passClerkStatuses[MAX_PASS_CLERKS];
Lock* passClerkLocks[MAX_PASS_CLERKS];
Condition* passClerkCVs[MAX_PASS_CLERKS];
int passClerkSSNs[MAX_PASS_CLERKS];
int passPunish[MAX_PASS_CLERKS];

ClerkStatus cashClerkStatuses[MAX_CASH_CLERKS];
Lock* cashClerkLocks[MAX_CASH_CLERKS];
Condition* cashClerkCVs[MAX_CASH_CLERKS];
int cashClerkSSNs[MAX_CASH_CLERKS];
int cashPunish[MAX_CASH_CLERKS];

int appFiled[MAX_CUSTOMERS];
int picFiled[MAX_CUSTOMERS];
int passFiled[MAX_CUSTOMERS];
int cashFiled[MAX_CUSTOMERS];

extern void CustomerRun(int);
extern void AppClerkRun(int);
extern void PicClerkRun(int);
extern void CashClerkRun(int);
extern void PassClerkRun(int);

void initializeClerkArrays(int, int, int, int);
void initializeCustomerArrays();

void Office() {
  
  int numAppClerks = 3; // WE WANT TO GET THESE FROM USER INPUT AND VALIDATE
  int numPicClerks = 3; //
  int numPassClerks = 3;
  int numCashClerks = 3;
  int numCustomers = 6;
  
  initializeClerkArrays(numAppClerks, numPicClerks, numPassClerks, numCashClerks);
  initializeCustomerArrays();
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

  for (int i = 0; i < numPassClerks; i++) {
    char* name = new char[20];
    snprintf(name, 20, "PassClerk%d", i);
    Thread* t = new Thread(name);
    t->Fork((VoidFunctionPtr)PassClerkRun, i);
    printf("Forked %s\n", name);
  }

  for (int i = 0; i < numCashClerks; i++) {
    char* name = new char[20];
    snprintf(name, 20, "CashClerk%d", i);
    Thread* t = new Thread(name);
    t->Fork((VoidFunctionPtr)CashClerkRun, i);
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

void initializeClerkArrays(int numAppClerks, int numPicClerks, int numPassClerks, int numCashClerks) {

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
    appClerkSSNs[i] = -1;
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
    picClerkSSNs[i] = -1;
    happyWithPhoto[i] = FALSE;
    
  }

  for (int i = 0; i < MAX_PASS_CLERKS; i++) {
    if (i < numPassClerks) {
      passClerkStatuses[i] = CLERK_AVAILABLE;
      char* name = new char[20];
      snprintf(name, 20, "PassClerkLock%d", i);
      passClerkLocks[i] = new Lock(name);
      name = new char[20];
      snprintf(name, 20, "PassClerkCV%d", i);
      passClerkCVs[i] = new Condition(name);
    }
    else {
      passClerkStatuses[i] = CLERK_INVALID;
      passClerkLocks[i] = NULL;
      passClerkCVs[i] = NULL;
    }
    passClerkSSNs[i] = -1;
    passPunish[i] = TRUE;
  }

  for (int i = 0; i < MAX_CASH_CLERKS; i++) {
    if (i < numCashClerks) {
      cashClerkStatuses[i] = CLERK_AVAILABLE;
      char* name = new char[20];
      snprintf(name, 20, "CashClerkLock%d", i);
      cashClerkLocks[i] = new Lock(name);
      name = new char[20];
      snprintf(name, 20, "CashClerkCV%d", i);
      cashClerkCVs[i] = new Condition(name);
    }
    else {
      cashClerkStatuses[i] = CLERK_INVALID;
      cashClerkLocks[i] = NULL;
      cashClerkCVs[i] = NULL;
    }
    cashClerkSSNs[i] = -1;
    cashPunish[i] = TRUE;
  }
}

void initializeCustomerArrays() {
  for (int i = 0; i < MAX_CUSTOMERS; i++) {
    appFiled[i] = FALSE;
    picFiled[i] = FALSE;
    passFiled[i] = FALSE;
    cashFiled[i] = FALSE;
  }
}
