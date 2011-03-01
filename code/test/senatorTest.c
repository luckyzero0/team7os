/*senatorTest.c*/
/* Test to show that customers
 * behave property in the presence
 * of the allpowerful senators.
 */

#include "office.h"
#include "syscall.h"
#include "utils.h"

int main(){
	TESTING = FALSE; /*too much text to make this true for this test*/
	OfficeTest6();
	Exit(0);	

}
