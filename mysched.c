/* Single Author Info:
 *   Equal Contribution of all
 * Group Member Info:
 *   abpatwar Aditya B Patwardhan
 *   savidhal Shivaji A Vidhale
 *   smnaik Sanskruti M Naik
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/syscall.h>
#include <sys/time.h>
#include <signal.h>
#include <math.h>
#include "myqueue.h"
#include "mythread.h"
#include "mymutex.h"
 
sigset_t new_mask, old_mask;
struct sigaction new_action, old_action;

void myhandler(int signal){
	
	mythread_queue_t rq_head,temp, runq_head;
	rq_head = *mythread_readyq();
	runq_head = *mythread_runq();
	temp = runq_head;
	mythread_t self = mythread_self();

	/*Every thread will set its reschudle flag on receiving a signal*/
	self->reschedule = 1;
	
	if(rq_head != NULL){
		(self->attr.attr)--; 
		/*On every signal (SIGALRM/SIGUSR1) the attr which denotes its remaining quanta is decremented
		This happens only if there is atleast one thread in readyq. 
		*/
		mythread_enter_kernel();
		if(signal == SIGALRM){
			do{
				if(temp->item != self){
					syscall(SYS_tkill, ((mythread_t)(temp->item))->tid, SIGUSR1);
				}
				temp = temp ->next;
			}
			while(temp!=runq_head);
		}

		 
		/*Only one thread will enter the kernel and reschedule the remaining ones which did not enter the kernel. 
		 * While doing so, the thread will set the reschedule flag of these threads to 0
		 * We need to check this condition before letting the threads enter the scheduler via mythread_leave_kernel() 
		*/
		 
		if(self->reschedule == 0){
			mythread_leave_kernel_nonpreemptive();
		}
		else{
			
			mythread_leave_kernel();
		}
	}
	/*Ready queue is empty so let this thread continue running without any interruptions*/
	return;
}

int timer_set = 0;

int mythread_init_sched(){

	/* Set the new_mask for SIGUSR1 and SIGALRM */
	mythread_t self = mythread_self();
	self->preemptions = 0;
	(self->attr).attr = pow(2.0,self->preemptions);

	sigemptyset(&new_mask);
	sigaddset(&new_mask,SIGUSR1);
	sigaddset(&new_mask,SIGALRM);
	sigprocmask(SIG_UNBLOCK,&new_mask,&old_mask);
	
	
	new_action.sa_handler = myhandler;
	new_action.sa_mask = new_mask;
	new_action.sa_flags = 0;

	/* Install signal handler for SIGUSR1 */
	if(sigaction(SIGUSR1,&new_action,&old_action)<0){
		perror("Error in installing handler for SIGUSR1\n");
		return -1;
	}
	
	/* Install signal handler for SIGALRM */
	if(sigaction(SIGALRM,&new_action,&old_action)<0){
		perror("Error in installing handler for SIGUSR1\n");
		return -1;
	}
	
	if(timer_set == 0){
		timer_set = 1;
		struct itimerval timer;
		struct timeval timerval;
		timerval.tv_sec=0;
		timerval.tv_usec=10000;
		timer.it_interval=timerval;
		timer.it_value=timerval;
		setitimer(ITIMER_REAL, &timer, NULL);
	}
	return 0;
}

int mythread_exit_sched(){
	
	/* Restore signal masks for the process */
	sigprocmask(SIG_SETMASK,&old_mask,&new_mask);
	/* Replace new handler with old handler for SIGUSR1 */
	if(sigaction(SIGUSR1,&old_action,&new_action)<0){
		perror("Error installing default handler for SIGUSR1\n");
		return -1;
	}
	/* Replace new handler with old handler for SIGALRM */
	if(sigaction(SIGALRM,&old_action,&new_action)<0){
		perror("Error installing default handler for SIGALRM\n");
		return -1;
	}
	return 0;
}

int mythread_scheduler(){
	mythread_t self = mythread_self();
	/*Limiting number of priority queues to 10 */

	if(self->preemptions < 10){
		self->preemptions += 1;
	}
	self->attr.attr = pow(2,self->preemptions); /*Since preemptions = invocation -1  */
	mythread_block(mythread_readyq(),1);
	return 0;	
}

int mythread_leave_kernel(){
	
	mythread_t self = mythread_self();

repeat:	
	
		if(self->reschedule == 1){ 
			self->reschedule = 0;
			/* Reschedule here*/
			if(mythread_readyq()!=NULL && self->attr.attr == 0){
				/*The threads time is up and since there's some thread waiting in readyq, it needs to
				preempt itself */
				mythread_scheduler();
			}
			else{
				/*If  Ready queue is empty or time is not up, continue executing
				*/
				mythread_leave_kernel_nonpreemptive();
			}
		}
		else{
			mythread_leave_kernel_nonpreemptive();
		}
	/* Check again in case of a rescheduling since leaving kernel */
	if(self->reschedule == 1){
		mythread_enter_kernel();
		goto repeat;
	}
	return 0;
}