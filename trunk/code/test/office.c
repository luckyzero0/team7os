/* office.c */
/*	This runs the office with the default values of 3 of each clerk,
*	20 customers, and 3 senators.  Everything should escape the office,
*	and the sum of cashier money should be $2300.
*/

#include "office.h"
#include "utils.h"
#include "syscall.h"



int TESTING = FALSE;
int FINISHED_FORKING = FALSE;
int needManager = FALSE;

int appClerkUID = 0;
int picClerkUID = 0;
int passClerkUID = 0;
int cashClerkUID = 0;
int customerSenatorUID = 0;

LockID appClerkUIDLock;
LockID picClerkUIDLock;
LockID passClerkUIDLock;
LockID cashClerkUIDLock;
LockID customerSenatorUIDLock;

int numAppClerks = 3; 
int numPicClerks = 3;
int numPassClerks = 3;
int numCashClerks = 3;
int numCustomers = 20;
int numSenators = 3;  

/* ApplicationClerk and PictureClerk lines*/
LockID appPicLineLock; 
ConditionID regAppLineCV; 
ConditionID privAppLineCV;
ConditionID regPicLineCV; 
ConditionID privPicLineCV;
int regAppLineLength = 0;
int privAppLineLength = 0;
int regPicLineLength = 0;
int privPicLineLength = 0;

LockID passLineLock;
ConditionID regPassLineCV;
ConditionID privPassLineCV;
int regPassLineLength = 0;
int privPassLineLength = 0;

LockID cashLineLock;
ConditionID regCashLineCV;

int regCashLineLength = 0;


/* Senators in office and waiting room */

ConditionID senatorWaitingRoomCV;
int senatorsInWaitingRoom = 0;


int senatorsInOffice = 0;

/* Customers in office and waiting room */

ConditionID customerWaitingRoomCV;
int customersInWaitingRoom = 0;

ConditionID managerWaitForCustomersCV;

LockID entryLock;
int customersInOffice = 0;



/* ApplicationClerk and PictureClerk States */
ClerkStatus appClerkStatuses[MAX_APP_CLERKS];
ClerkStatus picClerkStatuses[MAX_PIC_CLERKS];
LockID appClerkLocks[MAX_APP_CLERKS];
LockID picClerkLocks[MAX_PIC_CLERKS];
ConditionID appClerkCVs[MAX_APP_CLERKS];
ConditionID picClerkCVs[MAX_PIC_CLERKS];
int appClerkSSNs[MAX_APP_CLERKS];
int appClerkMoney[MAX_APP_CLERKS];
int appClerkBribed[MAX_APP_CLERKS];
int picClerkSSNs[MAX_PIC_CLERKS];
int picClerkMoney[MAX_PIC_CLERKS];
int picClerkBribed[MAX_PIC_CLERKS];
int happyWithPhoto[MAX_PIC_CLERKS];

ClerkStatus passClerkStatuses[MAX_PASS_CLERKS];
LockID passClerkLocks[MAX_PASS_CLERKS];
ConditionID passClerkCVs[MAX_PASS_CLERKS];
int passClerkSSNs[MAX_PASS_CLERKS];
int passClerkMoney[MAX_PASS_CLERKS];
int passClerkBribed[MAX_PASS_CLERKS];
int passPunish[MAX_PASS_CLERKS];

ClerkStatus cashClerkStatuses[MAX_CASH_CLERKS];
LockID cashClerkLocks[MAX_CASH_CLERKS];
ConditionID cashClerkCVs[MAX_CASH_CLERKS];
int cashClerkSSNs[MAX_CASH_CLERKS];
int cashClerkMoney[MAX_CASH_CLERKS];
int cashPunish[MAX_CASH_CLERKS];

int appFiled[MAX_CUSTOMERS];
int picFiled[MAX_CUSTOMERS];
int passFiled[MAX_CUSTOMERS];
int cashFiled[MAX_CUSTOMERS];



extern void CustomerRun();
extern void CustomerCashTest();/*for testing*/
extern void AppClerkRun();
extern void PicClerkRun();
extern void CashClerkRun();
extern void PassClerkRun();
extern void SenatorRun();
extern void ManagerRun();
extern void ManagerClerkTest(); /*for testing*/

void initializeClerkArrays();
void initializeCustomerArrays();


void initOfficeLocks(){

	appPicLineLock = CreateLock("appPicLineLock",14);
	regAppLineCV = CreateCondition("regAppLineCV",12);
	privAppLineCV = CreateCondition("privAppLineCV",13);
	regPicLineCV = CreateCondition("regPicLineCV",12);
	privPicLineCV = CreateCondition("privPicLineCV",13);

	passLineLock = CreateLock("passLineLock",12);
	regPassLineCV = CreateCondition("regPassLineCV",13);
	privPassLineCV = CreateCondition("privPassLineCV",14);
	
	cashLineLock = CreateLock("cashLineLock",12);
	regCashLineCV = CreateCondition("regCashLineCV",13);

	/* Senators in office and waiting room */
	senatorWaitingRoomLock = CreateLock("senatorWaitingRoomLock",22);
	senatorWaitingRoomCV = CreateCondition("senatorWaitingRoomCV",20);

	senatorOfficeLock = CreateLock("senatorOfficeLock",17);

	/* Customers in office and waiting room */
	customerWaitingRoomLock = CreateLock("customerWaitingRoomLock",23);
	customerWaitingRoomCV = CreateCondition("customerWaitingRoomCV",21);	

	managerWaitForCustomersCV = CreateCondition("managerWaitForCustomersCV", 25);

	entryLock = CreateLock("entryLock", 9);

	appClerkUIDLock = CreateLock("appClerkUIDLock",15);
	picClerkUIDLock = CreateLock("picClerkUIDLock",15);
	passClerkUIDLock = CreateLock("passClerkUIDLock",16);
	cashClerkUIDLock = CreateLock("cashClerkUIDLock",16);
	customerSenatorUIDLock = CreateLock("customerSenatorUIDLock",22);
}

void Office() {

	char* msg;
	int i;
	initOfficeLocks();
	msg = "Number of Customers = %d\n";
	printf(msg, numCustomers, 0, 0, "", "");
	msg = "Number of Senators = %d\n";
	printf(msg, numSenators, 0, 0, "", "");
	msg = "Number of ApplicationClerks = %d\n";
	printf(msg, numAppClerks, 0, 0, "", "");
	msg = "Number of PictureClerks = %d\n";
	printf(msg, numPicClerks, 0, 0, "", "");
	msg = "Number of PassportClerks = %d\n";
	printf(msg, numPassClerks, 0, 0, "", "");
	msg = "Number of Cashiers = %d\n";
	printf(msg, numCashClerks, 0, 0, "", "");

	initializeClerkArrays();
	initializeCustomerArrays();


	for (i = 0; i < numAppClerks; i++) {
		Fork(AppClerkRun);
	}	
	for (i = 0; i < numPicClerks; i++) {
		Fork(PicClerkRun);
	}
	for (i = 0; i < numPassClerks; i++) {
		Fork(PassClerkRun);
	}
	for (i = 0; i < numCashClerks; i++) {
		Fork(CashClerkRun);
	}

	/* Fork the customers*/
	for (i = 0; i < numCustomers; i++) {
		Fork(CustomerRun);
	}

	for (i = 0 ; i < numSenators; i++) {
		Fork(SenatorRun);
	}
	
	Fork(ManagerRun);

	FINISHED_FORKING = TRUE;
}

void initializeClerkArrays() {
	int i;
	char buf[50];
	char intbuf[5];
	for (i = 0; i < MAX_APP_CLERKS; i++) {
		if (i < numAppClerks) {
			appClerkStatuses[i] = CLERK_AVAILABLE;
			bzero(buf, 50);
			bzero(intbuf, 5);
			itoa(i, intbuf);
			strcpy(buf, "appClerk");
			strcat(buf, intbuf);
			printf("%s\n", 0,0,0,buf,"");
			appClerkLocks[i] = CreateLock(buf, strlen(buf));
			appClerkCVs[i] = CreateCondition(buf, strlen(buf));
			appClerkSSNs[i] = -1;
			appClerkMoney[i] = 0;
			appClerkBribed[i] = FALSE;
		} else {
			appClerkStatuses[i] = CLERK_INVALID;
		}
	}

	for (i = 0; i < MAX_PIC_CLERKS; i++) {
		if (i < numPicClerks) {
			picClerkStatuses[i] = CLERK_AVAILABLE;
			bzero(buf, 50);
			bzero(intbuf, 5);
			itoa(i, intbuf);
			strcpy(buf, "picClerk");
			strcat(buf, intbuf);
			printf("%s\n",0,0,0,buf,"");
			picClerkLocks[i] = CreateLock(buf, strlen(buf));
			picClerkCVs[i] = CreateCondition(buf, strlen(buf));
			picClerkSSNs[i] = -1;
			picClerkMoney[i] = 0;
			picClerkBribed[i] = FALSE;
			happyWithPhoto[i] = FALSE;
		} else {
			picClerkStatuses[i] = CLERK_INVALID;
		}

	}

	for ( i = 0; i < MAX_PASS_CLERKS; i++) {
		if (i < numPassClerks) {
			passClerkStatuses[i] = CLERK_AVAILABLE;
			bzero(buf, 50);
			bzero(intbuf, 5);
			itoa(i, intbuf);
			strcpy(buf, "passClerk");
			strcat(buf, intbuf);
			passClerkLocks[i] = CreateLock(buf, strlen(buf));
			passClerkCVs[i] = CreateCondition(buf, strlen(buf));
			passClerkSSNs[i] = -1;
			passClerkMoney[i] = 0;
			passClerkBribed[i] = FALSE;
			passPunish[i] = TRUE;
		} else {
			passClerkStatuses[i] = CLERK_INVALID;
		}
	}

	for ( i = 0; i < MAX_CASH_CLERKS; i++) {
		if (i < numCashClerks) {
			cashClerkStatuses[i] = CLERK_AVAILABLE;
			bzero(buf, 50);
			bzero(intbuf, 5);
			itoa(i, intbuf);
			strcpy(buf, "cashClerk");
			strcat(buf, intbuf);
			cashClerkLocks[i] = CreateLock(buf, strlen(buf));
			cashClerkCVs[i] = CreateCondition(buf, strlen(buf));
			cashClerkSSNs[i] = -1;
			cashClerkMoney[i] = 0;
			cashPunish[i] = TRUE;
		} else {
			cashClerkStatuses[i] = CLERK_INVALID;
		}
	}
}

void initializeCustomerArrays() {
	int i;
	for (i = 0; i < numCustomers+numSenators; i++) {
		appFiled[i] = FALSE;
		picFiled[i] = FALSE;
		passFiled[i] = FALSE;
		cashFiled[i] = FALSE;
	}
}

/*
void getInput(){			
char buffer[10];


bool invalid;
while(true)
{	
memset(buffer, NULL, 10);
invalid = false;
printf("\n# of ApplicationClerks = ");		
scanf("%s", buffer);		

//for whatever reason, nachos does not process cin.fail() checking
//like a normal OS.
for(int x = 0; x < sizeof(buffer)/sizeof(buffer[0]); x++) 
{
if(!isdigit(buffer[x]) && buffer[x] != NULL)
{
invalid = true;
printf("\nInvalid character. :: [%c]",buffer[x]);								
}

}
if(invalid)
continue;
else		
{
numAppClerks = atoi(buffer);	
if(numAppClerks < MAX_APP_CLERKS)								
break;		
else			
printf("\nNumber too big.");				
}		
}

while(true)
{	
memset(buffer, NULL, 10);
invalid = false;
printf("\n# of PictureClerks = ");		
scanf("%s", buffer);		

//for whatever reason, nachos does not process cin.fail() checking
//like a normal OS.
for(int x = 0; x < sizeof(buffer)/sizeof(buffer[0]); x++) 
{
if(!isdigit(buffer[x]) && buffer[x] != NULL)
{
invalid = true;
printf("\nInvalid character. :: [%c]",buffer[x]);								
}

}
if(invalid)
continue;
else		
{
numPicClerks = atoi(buffer);			
if(numPicClerks < MAX_PIC_CLERKS)								
break;		
else			
printf("\nNumber too big.");				
}		
}

while(true)
{	
memset(buffer, NULL, 10);
invalid = false;
printf("\n# of PassportClerks = ");		
scanf("%s", buffer);		

//for whatever reason, nachos does not process cin.fail() checking
//like a normal OS.
for(int x = 0; x < sizeof(buffer)/sizeof(buffer[0]); x++) 
{
if(!isdigit(buffer[x]) && buffer[x] != NULL)
{
invalid = true;
printf("\nInvalid character. :: [%c]",buffer[x]);								
}

}
if(invalid)
continue;
else		
{
numPassClerks = atoi(buffer);			
if(numPassClerks < MAX_PASS_CLERKS)								
break;		
else			
printf("\nNumber too big.");				
}		
}

while(true)
{	
memset(buffer, NULL, 10);
invalid = false;
printf("\n# of CashierClerks = ");		
scanf("%s", buffer);		

//for whatever reason, nachos does not process cin.fail() checking
//like a normal OS.
for(int x = 0; x < sizeof(buffer)/sizeof(buffer[0]); x++) 
{
if(!isdigit(buffer[x]) && buffer[x] != NULL)
{
invalid = true;
printf("\nInvalid character. :: [%c]",buffer[x]);								
}

}
if(invalid)
continue;
else		
{
numCashClerks = atoi(buffer);			
if(numCashClerks < MAX_CASH_CLERKS)								
break;		
else			
printf("\nNumber too big.");				
}		
}

while(true)
{	
memset(buffer, NULL, 10);
invalid = false;
printf("\n# of Customers = ");		
scanf("%s", buffer);		

//for whatever reason, nachos does not process cin.fail() checking
//like a normal OS.
for(int x = 0; x < sizeof(buffer)/sizeof(buffer[0]); x++) 
{
if(!isdigit(buffer[x]) && buffer[x] != NULL)
{
invalid = true;
printf("\nInvalid character. :: [%c]",buffer[x]);								
}

}
if(invalid)
continue;
else		
{
numCustomers = atoi(buffer);			
if(numCustomers < MAX_CUSTOMERS)								
break;		
else			
printf("\nNumber too big.");				
}		
}

while(true)
{	
memset(buffer, NULL, 10);
invalid = false;
printf("\n# of Senators = ");		
scanf("%s", buffer);		

//for whatever reason, nachos does not process cin.fail() checking
//like a normal OS.
for(int x = 0; x < sizeof(buffer)/sizeof(buffer[0]); x++) 
{
if(!isdigit(buffer[x]) && buffer[x] != NULL)
{
invalid = true;
printf("\nInvalid character. :: [%c]",buffer[x]);								
}

}
if(invalid)
continue;
else		
{
numSenators = atoi(buffer);			
if(numSenators+numCustomers < MAX_CUSTOMERS)								
break;		
else			
printf("\nNumber too big. Total Customers + Total Senators must be < %d", MAX_CUSTOMERS);				
}		
}

}
*/

void tprintf(char* formatString, int arg1, int arg2, int arg3, char* arg4, char* arg5) {
	if (TESTING) {
		printf(formatString, arg1, arg2, arg3, arg4, arg5);
	}
}


/*
 * Test to ensure that customers get in line properly.
 */
void OfficeTest1(){
	printf("Initializing the office...\n",0,0,0,"","");
	numAppClerks = 0; 
	numPicClerks = 0;
	numPassClerks = 0;
	numCashClerks = 0;
	numCustomers = 6;
	numSenators = 0;
	privAppLineLength = 3;
	privPicLineLength = 0;
	Office();
	Exit(0);	
}

/*
 * Test to ensure that the manager cannot multitask.
 */ 
void OfficeTest2(){
	numAppClerks = 2; 
	numPicClerks = 2;
	numPassClerks = 2;
	numCashClerks = 2;
	numCustomers = 0;
	numSenators = 0;
	
	Office();
	
	appClerkMoney[0] = 500;
	picClerkMoney[0] = 500;
	passClerkMoney[0] = 500;
	cashClerkMoney[0] = 0; /*to trigger the manager-collection sequence*/
	appClerkMoney[1] = 500;
	picClerkMoney[1] = 500;
	passClerkMoney[1] = 500;
	cashClerkMoney[1] = 0;
	Exit(0);
}


/*
 *	Test to demonstrate customers entering and leaving the passport office,
 *  but only interactions with the cashier clerk, namely that they only leave once
 *  they received their passport, and that the cashclerk only interacts with them
 *  one at a time.
*/
void OfficeTest3(){
	char* msg;
	int i;
	numAppClerks = 0; 
	numPicClerks = 0;
	numPassClerks = 0;
	numCashClerks = 1;
	numCustomers = 3;
	numSenators = 0;
	initOfficeLocks();
	msg = "Number of Customers = %d\n";
	printf(msg, numCustomers, 0, 0, "", "");
	msg = "Number of Senators = %d\n";
	printf(msg, numSenators, 0, 0, "", "");
	msg = "Number of ApplicationClerks = %d\n";
	printf(msg, numAppClerks, 0, 0, "", "");
	msg = "Number of PictureClerks = %d\n";
	printf(msg, numPicClerks, 0, 0, "", "");
	msg = "Number of PassportClerks = %d\n";
	printf(msg, numPassClerks, 0, 0, "", "");
	msg = "Number of Cashiers = %d\n";
	printf(msg, numCashClerks, 0, 0, "", "");

	initializeClerkArrays();
	initializeCustomerArrays();
	
	for (i = 0; i < numCustomers; i++) {
		Fork(CustomerCashTest);
	}
	for (i = 0; i < numCashClerks; i++) {
		Fork(CashClerkRun);
	}	

	passFiled[0] = TRUE;
	passFiled[1] = TRUE;
	passFiled[2] = TRUE;
	Exit(0);
}

/*
 * Demonstrates clerks going on break when
 * there are no customers in their lines.
 */ 
void OfficeTest4(){	
	numAppClerks = 2; 
	numPicClerks = 2;
	numPassClerks = 2;
	numCashClerks = 2;
	numCustomers = 0;
	numSenators = 0;
	Office();
	Exit(0);
}

/*
 * This test shows that the manager
 * wakes up clerks if they have people 
 * in their lines.
*/
void OfficeTest5(){
	char* msg;
	int i;
	numAppClerks = 1; 
	numPicClerks = 1;
	numPassClerks = 1;
	numCashClerks = 1;
	numCustomers = 0;
	numSenators = 0;
	
	initOfficeLocks();
	msg = "Number of Customers = %d\n";
	printf(msg, numCustomers, 0, 0, "", "");
	msg = "Number of Senators = %d\n";
	printf(msg, numSenators, 0, 0, "", "");
	msg = "Number of ApplicationClerks = %d\n";
	printf(msg, numAppClerks, 0, 0, "", "");
	msg = "Number of PictureClerks = %d\n";
	printf(msg, numPicClerks, 0, 0, "", "");
	msg = "Number of PassportClerks = %d\n";
	printf(msg, numPassClerks, 0, 0, "", "");
	msg = "Number of Cashiers = %d\n";
	printf(msg, numCashClerks, 0, 0, "", "");

	initializeClerkArrays();
	initializeCustomerArrays();

	for (i = 0; i < numAppClerks; i++) {
		Fork(AppClerkRun);
	}	
	for (i = 0; i < numPicClerks; i++) {
		Fork(PicClerkRun);
	}
	for (i = 0; i < numPassClerks; i++) {
		Fork(PassClerkRun);
	}
	for (i = 0; i < numCashClerks; i++) {
		Fork(CashClerkRun);
	}
	
	
	
	Fork(ManagerClerkTest);
	

	Exit(0);
}

/*
 * Demonstrates proper customer behavior when senators arrive
 * and when they leave
 *
 */ 
void OfficeTest6(){	
	numAppClerks = 1; 
	numPicClerks = 1;
	numPassClerks = 1;
	numCashClerks = 1;
	numCustomers = 2;
	numSenators = 1;
	Office();
	Exit(0);
}