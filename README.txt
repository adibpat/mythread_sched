/* Single Author Info:
 *   smnaik Sanskruti M Naik
/* Group Info:
*  abpatwar Aditya B Patwardhan
*  savidhal Shivaji A Vidhale
*  smnaik Sanskruti M Naik
*/

Introduction:
-------------

This project is an implementation of feedback prioritized preemptive scheduling with time slice of 2^(i-1)ms for the i-th invocation in wall-clock time (SIGALRM) for the user-level thread library -> mythread.a


We have provided the following methods in mysched.c:
-> mythread_init_sched()
-> mythread_exit_sched()
-> mythread_scheduler()
-> mythread_leave_kernel()
-> Signal Handler

We have edited the myqueue.c file provided to us to implement a prioritized queue. 


We have also provided a test file to test the methods in mysched.c
-> mytest.c

Method Implementation:
----------------------

1. mythread_init_sched()
In this method, we have added two signals (SIGALRM and SIGUSR1) to the signal set of the process and UNBLOCKED both the signals for the process.
The handler for the two signals has also been installed in this method and the timer value for SIGALRM has been set to 10 ms.


2. mythread_exit_sched()
In this method, we have restored the default signal masks for ths process and the default handler for SIGALRM and SIGUSR1.


3. mythread_scheduler()
In this method, the actual blocking of the threads takes place.

For the thread entering the scheduler, if the number of pre-emptions are less than 10, then the number of pre-emptions is incremented by 1 and the time slice is updated according to the new pre-emption vlaue.
Then, the thread is blocked.


4. mythread_leave_kernel()
In this method, the scheduler gets invoked.

For the thread which enters the method, 'reschedule' is set to 0. 
If the ready queue is not empty and the time-slice alloted to the thread is over then the thread invokes the scheduler. 
Once again, the 'reschedule' is checked in this method to consider the case when a thread receives a signal when it is inside the signal handler.
If this condition is true, then the leave kernel procedure is repeated again.


5. Signal Handler
The method takes one argument - signal (SIGALRM/SIGUSR1).

For each thread entering this handler, 'reschedule' is initially set to 1.
If the ready queue is not empty, the thread enters the kernel.
If the signal handler has been invoked due to the signal - SIGARLM, then the thread which has entered the kernel sends SIGUSR1 to all the other threads in the run queue except for itself. 
For the thread with 'reschedule' set to 1, the method mythread_leave_kernel() (defined by us) is invoked.


How to Build:
------------

Refer to the following commands for building ->

Compiling:
     $ make
# This is equivalent to "make all".
# It compiles all the code files, creates the corresponding object files and builds 
# the library file "mythread.a”. It also builds the mytest.c file - a sample
# test program provided - which tests out the methods in mysched.c.
		     
Cleaning:
     $ make clean

Test file:
     $ make test


Test case:
----------
We have also provided a test file named mytest.c

In order to test mysched.c, 10 threads are created and the concurrency level is set to 4.

The threads call the thread function in which the threads increment the local variable c by 1 while c is less than 500.

We have used while loops so that the threads can run longer to simulate pre-emptions.
If for the thread, the value of the global variable is even, then the while loop runs 100000 times and if it is odd then the while loop runs 200000 times.

After the value of c exceeds 500, the test program prints the threads along with their IDs and the number of preemptions.




