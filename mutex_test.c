#include <stdio.h>
#include <stdlib.h>
#include "mymutex.h"
#include "mythread.h"
#include <unistd.h>

mythread_t thread1[10];
mythread_t thread2[20];
int count=0;

mythread_mutex_t locks;

void* doSomeThing1(void *arg)
{   
    mythread_mutex_lock(&locks);
    //printf("Thread %ld inside function\n",mythread_self()->tid);
    count++;
    mythread_mutex_unlock(&locks);
    while(1);
    //printf("Exiting thread %ld\n",mythread_self()->tid);
    mythread_exit(NULL);
}

void* doSomeThing2(void *arg)
{   
    mythread_mutex_lock(&locks);
    sleep(1);
    count++;
    mythread_mutex_unlock(&locks);

    mythread_exit(NULL);
}



int main(void)
{
    int err1, err2;
    int i;

    /*if(mythread_mutex_init(&locks, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return 1;
    }*/
    printf("The main thread ID is %ld\n",getpid());
    mythread_setconcurrency(1);
    for(i=0;i<10;i++){
        err1 = mythread_create(&thread1[i], NULL, doSomeThing1, NULL);
        if (err1 != 0) printf("%d",err1);
        //err2 = mythread_create(&thread2[i], NULL, doSomeThing2, NULL);
        //if (err2 != 0) printf("%d",err2);
    }
            
    for (i = 0; i<10; i++)
    {
        mythread_join(thread1[i],NULL);
    }

    /*for (i = 0; i<20; i++)
    {
        mythread_join(thread2[i],NULL);
    }*/
    
    printf("Final count value is %d\n",count);

    return 0;
}
