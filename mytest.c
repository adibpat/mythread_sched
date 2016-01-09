/* Single Author Info:
 *   smnaik Sanskruti M Naik
 * Group Info:
 *   abpatwar Aditya B Patwardhan
 *   savidhal Shivaji A Vidhale
 *	 smnaik Sanskruti M Naik
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mythread.h"


mythread_t threads[10];

/* Thread function */
void *thread_function ()
{
	int c = 0;
	while (1)
	{
		if (c < 3000)
		{
			if (c%2 == 0)
			{	
				int k = 1;
				while(k<100000) //While loops to simulate preemptions by making threads run for longer
					k++; 
					
				c++; 		// Each thread will update this variable
								
			}
			else
			{
				int k = 1;
				while(k<200000) //While loops to simulate preemptions by making threads run for longer
					k++;
				c++; 		
			}

			
		}
		else 
			break;
	
	}

	mythread_enter_kernel();
	printf("Thread with tid %ld has been pre-empted: %d times! \n", mythread_self(), mythread_self()->preemptions);
	mythread_leave_kernel_nonpreemptive();

}



int main(void){
  
  int rc1, rc2;
  int i;
    
  mythread_setconcurrency(4);

	/*	Creating 10 threads */  
   for ( i = 0; i<10; i++)
    {
        rc1 = mythread_create(&threads[i], NULL, thread_function, (void *)NULL);
        if (rc1) ("ERROR; return code from pthread_create() is %d\n", rc1);
    }

   /* Main will now wait for 10 threads */
  for(i = 0; i<10; i++)
  {
   		rc2 = mythread_join(threads[i], NULL);
    	if (rc2) ("ERROR; return code from pthread_join() is %d\n", rc2);
  }
  
  return 0;

}