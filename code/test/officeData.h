/*officeData.h
 * Contains all the data declarations
 * needed for the office in one spot.
 */
 #include "utils.h"
 #include "syscall.h"

#ifndef OFFICE_DATA_H
#define OFFICE_DATA_H

MonitorID TESTING; //= FALSE;
MonitorID FINISHED_FORKING; //= FALSE;
MonitorID needManager; //= FALSE;

MonitorID appClerkUID; //= 0;
MonitorID picClerkUID; //= 0;
MonitorID passClerkUID; //= 0;
MonitorID cashClerkUID; //= 0;
MonitorID customerSenatorUID; //= 0;

LockID appClerkUIDLock;
LockID picClerkUIDLock;
LockID passClerkUIDLock;
LockID cashClerkUIDLock;
LockID customerSenatorUIDLock;

MonitorID numAppClerks; //= 3; 
MonitorID numPicClerks; //= 3;
MonitorID numPassClerks; //= 3;
MonitorID numCashClerks; //= 3;
MonitorID numCustomers; //= 20;
MonitorID numSenators; //= 3;  

/* ApplicationClerk and PictureClerk lines*/
LockID appPicLineLock; 
ConditionID regAppLineCV; 
ConditionID privAppLineCV;
ConditionID regPicLineCV; 
ConditionID privPicLineCV;
MonitorID regAppLineLength;//; //= 0;
MonitorID privAppLineLength;//; //= 0;
MonitorID regPicLineLength;//; //= 0;
MonitorID privPicLineLength;//; //= 0;

LockID passLineLock;
ConditionID regPassLineCV;
ConditionID privPassLineCV;
MonitorID regPassLineLength;//; //= 0;
MonitorID privPassLineLength;//; //= 0;

LockID cashLineLock;
ConditionID regCashLineCV;

MonitorID regCashLineLength;//; //= 0;


/* Senators in office and waiting room */

ConditionID senatorWaitingRoomCV;
MonitorID senatorsInWaitingRoom; //= 0;


MonitorID senatorsInOffice; //= 0;

/* Customers in office and waiting room */

ConditionID customerWaitingRoomCV;
MonitorID customersInWaitingRoom; //= 0;

ConditionID managerWaitForCustomersCV;

LockID entryLock;
MonitorID customersInOffice; //= 0;



/* ApplicationClerk and PictureClerk States */
ClerkStatus appClerkStatuses;//[MAX_APP_CLERKS];
ClerkStatus picClerkStatuses;//[MAX_PIC_CLERKS];
LockID appClerkLocks;//[MAX_APP_CLERKS];
LockID picClerkLocks;//[MAX_PIC_CLERKS];
ConditionID appClerkCVs;//[MAX_APP_CLERKS];
ConditionID picClerkCVs;//[MAX_PIC_CLERKS];d
MonitorArrayID appClerkSSNs;//[MAX_APP_CLERKS];
MonitorArrayID appClerkMoney;//[MAX_APP_CLERKS];
MonitorArrayID appClerkBribed;//[MAX_APP_CLERKS];
MonitorArrayID picClerkSSNs;//[MAX_PIC_CLERKS];
MonitorArrayID picClerkMoney;//[MAX_PIC_CLERKS];
MonitorArrayID picClerkBribed;//[MAX_PIC_CLERKS];
MonitorArrayID happyWithPhoto;//[MAX_PIC_CLERKS];

ClerkStatus passClerkStatuses;//[MAX_PASS_CLERKS];
LockID passClerkLocks;//[MAX_PASS_CLERKS];
ConditionID passClerkCVs;//[MAX_PASS_CLERKS];
MonitorArrayID passClerkSSNs;//[MAX_PASS_CLERKS];
MonitorArrayID passClerkMoney;//[MAX_PASS_CLERKS];
MonitorArrayID passClerkBribed;//[MAX_PASS_CLERKS];
MonitorArrayID passPunish;//[MAX_PASS_CLERKS];

ClerkStatus cashClerkStatuses;//[MAX_CASH_CLERKS];
LockID cashClerkLocks;//[MAX_CASH_CLERKS];
ConditionID cashClerkCVs;//[MAX_CASH_CLERKS];
MonitorArrayID cashClerkSSNs;//[MAX_CASH_CLERKS];
MonitorArrayID cashClerkMoney;//[MAX_CASH_CLERKS];
MonitorArrayID cashPunish;//[MAX_CASH_CLERKS];

MonitorArrayID appFiled;//[MAX_CUSTOMERS];
MonitorArrayID picFiled;//[MAX_CUSTOMERS];
MonitorArrayID passFiled;//[MAX_CUSTOMERS];
MonitorArrayID cashFiled;//[MAX_CUSTOMERS];








#endif
