#include "officeData.h"
#include "utils.h"

char* getCustomerType() {
	char* result;

	Acquire(entryLock);
	if (GetMonitor(senatorsInOffice) > 0){
		result = "Senator";
	} else {
		result = "Customer";
	}
	Release(entryLock);

	return result;
}

int initCashClerkData() {
	int myUID;
	char* name;
	int id;

	/* line locks, conditions, and monitors */
	cashLineLock = CreateLock("cashLineLock",12);
	regCashLineCV = CreateCondition("regCashLineCV",13);

	regCashLineLength = CreateMonitor("regCashLineLength",17);

	/* uid lock and monitors */
	cashClerkUIDLock = CreateLock("cashClerkUIDLock",15);
	cashClerkUID = CreateMonitor("cashClerkUID", 11);

	/* cash filed monitor array */
	passFiled = CreateMonitorArray("passFiled", 9, NUM_CUSTOMERS, FALSE);
	cashFiled = CreateMonitorArray("cashFiled", 9, NUM_CUSTOMERS, FALSE);

	/* entry lock so we can check whether current customres are customers or senators */
	entryLock = CreateLock("entryLock", 9);
	senatorsInOffice = CreateMonitor("senatorsInOffice", 16);

	/* all the arrays */
	cashClerkStatuses = CreateMonitorArray("cashClerkStatuses", 17, NUM_OF_EACH_TYPE_OF_CLERK, 0);/* of monitors, default to 0 == CLERK_NOT_AVAILABLE*/
	cashClerkLocks = CreateMonitorArray("cashClerkLocks", 14, NUM_OF_EACH_TYPE_OF_CLERK, -1);/* of lockIDs, default to -1*/
	cashClerkCVs = CreateMonitorArray("cashClerkCVs", 12, NUM_OF_EACH_TYPE_OF_CLERK, -1);/* of conditionIDs, default to -1*/
	cashClerkSSNs = CreateMonitorArray("cashClerkSSNs", 13, NUM_OF_EACH_TYPE_OF_CLERK, -1);/* of monitors, default to -1*/
	cashClerkMoney = CreateMonitorArray("cashClerkMoney", 14, NUM_OF_EACH_TYPE_OF_CLERK, 0);/* of monitors, default to 0*/
	cashPunish = CreateMonitorArray("cashPunish", 10, NUM_OF_EACH_TYPE_OF_CLERK, FALSE); /* of monitors, default to FALSE */

	/* obtain SSN */
	Acquire(cashClerkUIDLock);
	myUID = GetMonitor(cashClerkUID);
	SetMonitor(cashClerkUID, myUID + 1);
	Release(cashClerkUIDLock);

	/* initialize necessary values for arrays at my index */
	name = "cashClerk?Lock";
	name[9] = myUID + '0';
	id = CreateLock(name, 14);
	SetMonitorArrayValue(cashClerkLocks, myUID, id); /* set the value at my index in the lock array, to the handle to my lock */

	name = "cashClerk?Condition";
	name[9] = myUID + '0';
	id = CreateCondition(name, 19);
	SetMonitorArrayValue(cashClerkCVs, myUID, id); /* now set the value for our condition in the condition array */

	return myUID;
}

void CashClerkRun(){
	int SSN;
	int index;
	LockID myLockID;
	ConditionID myConditionID;

	

	index = initCashClerkData();
	myLockID = GetMonitorArrayValue(cashClerkLocks, index);
	myConditionID = GetMonitorArrayValue(cashClerkCVs, index);

	while (TRUE){		
		/*cashLineLock->Acquire();*/
		Acquire(cashLineLock);		

		/*Checking if anyone is in line*/
		if (GetMonitor(regCashLineLength) > 0){
			tprintf("CashClerk %d: there are people in line.\n", index,0,0,"","");

			tprintf("CashClerk %d: has spotted %s in regCashLine (length = %d)\n",index, GetMonitor(regCashLineLength), 0, getCustomerType(),"");
			SetMonitor(regCashLineLength, GetMonitor(regCashLineLength) - 1);
			tprintf("CashClerk %d: Becoming Available!\n",index,0,0,"","");
			SetMonitorArrayValue(cashClerkStatuses, index, CLERK_AVAILABLE);
			tprintf("CashClerk %d: Signaling Condition variable (length of reg line is now %d)\n", index,GetMonitor(regCashLineLength),0,"","");
			/*regCashLineCV->Signal(cashLineLock);*/
			Signal(regCashLineCV, cashLineLock);

			/*Customer/Senator - Clerk interaction*/
			tprintf("CashClerk %d: Acquiring my own lock\n",index,0,0,"","");
			/*cashClerkLocks[index]->Acquire();*/

			
			Acquire(myLockID);
			tprintf("CashClerk %d: Releasing cashLineLock\n",index,0,0,"","");
			/*cashLineLock->Release();*/
			Release(cashLineLock);
			tprintf("CashClerk %d: Putting myself to sleep...\n",index,0,0,"","");
			/*cashClerkCVs[index]->Wait(cashClerkLocks[index]);*/
			Wait(myConditionID, myLockID);
			tprintf("CashClerk %d: Just woke up!\n",index,0,0,"","");

			SSN = GetMonitorArrayValue(cashClerkSSNs, index); /* use this locally instead of calling get on the array all the time */

			if (GetMonitorArrayValue(passFiled, SSN) == FALSE){
				printf("CashierClerk [%d] gives invalid certification to %s with SSN %d\n", index, SSN, 0, getCustomerType(),""); 
				printf("CashierClerk [%d] punishes %s with SSN %d to wait\n", index, SSN, 0, getCustomerType(),""); 
				SetMonitorArrayValue(cashPunish, index, TRUE);
			} else {
				tprintf("CashClerk %d: %s with SSN %d has everything filed correctly!\n",index,SSN,0,getCustomerType(),"");
				printf("Cashier [%d] gives valid certification to %s with SSN %d\n", index, SSN, 0, getCustomerType(),"");
				printf("Cashier [%d] records %s with SSN %d's passport\n", index, SSN, 0, getCustomerType(),"");
				printf("Cashier [%d] has recorded the passport for %s with SSN %d\n", index, SSN, 0, getCustomerType(),"");
				SetMonitorArrayValue(cashPunish, index, FALSE);
				SetMonitorArrayValue(cashFiled, SSN, TRUE);

				printf("Cashier [%d] accepts money = 100 from %s with SSN %d\n", index, SSN, 0, getCustomerType(),"");
				Signal(myConditionID, myLockID);
				Wait(myConditionID, myLockID);
				tprintf("CashClerk %d: Total money collected: $%d\n",index, GetMonitorArrayValue(cashClerkMoney, index),0,"","");

			}

			tprintf("CashClerk %d: Signaling my cashClerkCV\n", index,0,0,"","");
			Signal(myConditionID, myLockID);
			tprintf("CashClerk %d: Releasing my own lock\n", index,0,0,"","");
			Release(myLockID);

		}
		else{ /*No one in line*/
			/*cashLineLock->Release();*/
			Release(cashLineLock);
			/*cashClerkLocks[index]->Acquire();*/
			/*tprintf("CashierClerk [%d] acquiring his own lock\n", index,0,0,"","");*/
			Acquire(myLockID);
			printf("CashierClerk [%d] is going on break\n", index,0,0,"","");
			SetMonitorArrayValue(cashClerkStatuses, index, CLERK_ON_BREAK);
			/*cashClerkCVs[index]->Wait(cashClerkLocks[index]);*/
			Wait(myConditionID, myLockID);
			printf("CashierClerk [%d] returned from break\n", index,0,0,"","");
			/*cashClerkLocks[index]->Release();*/
			Release(myLockID);
		}
		Yield();
	}
	printf("About to exit!", 0,0,0,"","");
}

int main() {
	CashClerkRun();
	Exit(0);
}