/*officeData.h
 * Contains all the data declarations
 * needed for the office in one spot.
 */
 #include "utils.h"
 #include "syscall.h"

#ifndef OFFICE_DATA_H
#define OFFICE_DATA_H

MonitorID appClerkUID;
MonitorID picClerkUID;
MonitorID passClerkUID;
MonitorID cashClerkUID;
MonitorID customerSenatorUID;

LockID appClerkUIDLock;
LockID picClerkUIDLock;
LockID passClerkUIDLock;
LockID cashClerkUIDLock;
LockID customerSenatorUIDLock;

#define NUM_OF_EACH_TYPE_OF_CLERK 3
#define NUM_CUSTOMERS 20
#define NUM_SENATORS 3

enum ClerkStatus {CLERK_INVALID, CLERK_AVAILABLE, CLERK_ON_BREAK, CLERK_BUSY, CLERK_COMING_BACK};

/*
MonitorID numAppClerks; //= 3; 
MonitorID numPicClerks; //= 3;
MonitorID numPassClerks; //= 3;
MonitorID numCashClerks; //= 3;
MonitorID numCustomers; //= 20;
MonitorID numSenators; //= 3;
*/

/* ApplicationClerk and PictureClerk lines*/
LockID appPicLineLock; 
ConditionID regAppLineCV; 
ConditionID privAppLineCV;
ConditionID regPicLineCV; 
ConditionID privPicLineCV;
MonitorID regAppLineLength;
MonitorID privAppLineLength;
MonitorID regPicLineLength;
MonitorID privPicLineLength;

LockID passLineLock;
ConditionID regPassLineCV;
ConditionID privPassLineCV;
MonitorID regPassLineLength;
MonitorID privPassLineLength;

LockID cashLineLock;
ConditionID regCashLineCV;

MonitorID regCashLineLength;


/* Senators in office and waiting room */

ConditionID senatorWaitingRoomCV;
MonitorID senatorsInWaitingRoom;


MonitorID senatorsInOffice;

/* Customers in office and waiting room */

ConditionID customerWaitingRoomCV;
MonitorID customersInWaitingRoom;

ConditionID managerWaitForCustomersCV;

LockID entryLock;
MonitorID customersInOffice; //= 0;



/* ApplicationClerk and PictureClerk States */
MonitorArrayID appClerkStatuses;
MonitorArrayID appClerkLocks;
MonitorArrayID appClerkCVs;
MonitorArrayID appClerkSSNs;
MonitorArrayID appClerkMoney;
MonitorArrayID appClerkBribed;

MonitorArrayID picClerkStatuses;
MonitorArrayID picClerkLocks;
MonitorArrayID picClerkCVs;
MonitorArrayID picClerkSSNs;
MonitorArrayID picClerkMoney;
MonitorArrayID picClerkBribed;
MonitorArrayID happyWithPhoto;

MonitorArrayID passClerkStatuses;
MonitorArrayID passClerkLocks;
MonitorArrayID passClerkCVs;
MonitorArrayID passClerkSSNs;
MonitorArrayID passClerkMoney;
MonitorArrayID passClerkBribed;
MonitorArrayID passPunish;

MonitorArrayID cashClerkStatuses;
MonitorArrayID cashClerkLocks;
MonitorArrayID cashClerkCVs;
MonitorArrayID cashClerkSSNs;
MonitorArrayID cashClerkMoney;
MonitorArrayID cashPunish;

MonitorArrayID appFiled; /* based on NUM_CUSTOMERS */
MonitorArrayID picFiled;
MonitorArrayID passFiled;
MonitorArrayID cashFiled;








#endif
