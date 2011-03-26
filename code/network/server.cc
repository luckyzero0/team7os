//Server.cc
/* The main routine for the server
 * Listens for msgs from clients
 */
 
#include "copyright.h"

#include "system.h"
#include "network.h"
#include "post.h"
#include "interrupt.h"

void RunServer(void){
	printf("Server online.\n");

	printf("Listening for clients on the network...\n");

	while(true){

		printf("IMPLEMENT THIS SHIZ\n");
		break;
	}
	
	interrupt->Halt();
}
