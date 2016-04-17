// threadtest.cc 
//	Simple test case for the threads assignment.
//
//	Create two threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield, 
//	to illustratethe inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "synch.h"
#include "elevatortest.h"
#define BUFFERSIZE 5

// testnum is set in main.cc
int testnum = 1;

Semaphore * pLock = new Semaphore("PLOCK", BUFFERSIZE);
Semaphore * cLock = new Semaphore("CLOCK", 0);
Semaphore * mutex = new Semaphore("MUTEX", 1);

int buffer = 0;
Lock * lmutex = new Lock("MUTEX");
Lock * lcLock = new Lock("CLOCK");
Lock * lpLock = new Lock("PLOCK");
Condition * lcCond = new Condition("CCOND");
Condition * lpCond = new Condition("PCOND");
Barrier * barrier = new Barrier(3);

//----------------------------------------------------------------------
// SimpleThread
// 	Loop 5 times, yielding the CPU to another ready thread 
//	each iteration.
//
//	"which" is simply a number identifying the thread, for debugging
//	purposes.
//----------------------------------------------------------------------

void
SimpleThread(int which)
{
    int num;
    
    for (num = 0; num < 5; num++) {
	printf("*** thread %d looped %d times\n", which, num);
        currentThread->Yield();
    }
}

//----------------------------------------------------------------------
// ThreadTest1
// 	Set up a ping-pong between two threads, by forking a thread 
//	to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------

void
ThreadTest1()
{
    DEBUG('t', "Entering ThreadTest1\n");

    Thread *t = new Thread("forked thread");

    t->Fork(SimpleThread, (void*)1);
    SimpleThread(0);
}

void doVoid(int tid) {
    printf("forked thread run with tid %d\n", tid);
    return;
}

void ThreadTest2() {
    DEBUG('t', "Entering ThreadTest2\n");
    for(int i = 0; i <= 129; ++i) {
        Thread *t = new Thread("forked thread");
        t->Fork(doVoid, t->getTID());
        currentThread->Yield();
    }
}

void LongThread(int tid) {
    DEBUG('t', "begin %d\n", tid);
    int round = 0;
    while(currentThread->getCurrentTick() < 90) {
        DEBUG('t', "in thread %d, loop %d times\n", tid, currentThread->getCurrentTick());
        if(round != currentThread->getCurrentTick()) {
            round = currentThread->getCurrentTick();
            printf("in thread %d, loop %d times\n", tid, currentThread->getCurrentTick());
        }

        IntStatus oldLevel = interrupt->SetLevel(IntOff);
        (void) interrupt->SetLevel(oldLevel);
    }
    printf("Thread %d finished\n", tid);
    return;
}

void ThreadTest3() {
    printf("Creating thread 1\n");
    Thread *t1 = new Thread("forked 1 with 7", 8);
    t1->Fork(LongThread, 1);
    printf("Creating thread 2\n");
    Thread *t2 = new Thread("forked 2 with 8", 9);
    t2->Fork(LongThread, 2);
    printf("Creating thread 3\n");
    Thread *t3 = new Thread("forked 3 with 5", 7);
    t3->Fork(LongThread, 3);
    printf("Creating thread 4\n");
    Thread *t4 = new Thread("forked 4 with 2", 5);
    t4->Fork(LongThread, 4);
    printf("Creating thread 5\n");
    Thread *t5 = new Thread("forked 5 with 3", 6);
    t5->Fork(LongThread, 5);
}

void Producer(int tid) {
    for(int i = 0; i < 10; ++i) {
        //printf("thereinp\n");
        pLock->P();
        mutex->P();
        printf("producing %d\n", i);
        mutex->V();
        cLock->V();
        //printf("tohere1\n");
    //    lmutex->Acquire();
    //    while(buffer == BUFFERSIZE) {
    //        lpLock->Acquire();
    //        lmutex->Release();
    //        lpCond->Wait(lpLock);
    //        lmutex->Acquire();
    //        lpLock->Release();
    //    }
    //    buffer++;
    //    printf("producing! %d products left\n", buffer);
    //    lcLock->Acquire();
    //    lcCond->Signal(lcLock);
    //    lcLock->Release();
    //    lmutex->Release();
    //    currentThread->Yield();
    }
}

void Consumer(int tid) {
    for(int i = 0; i < 10; ++i) {
        //printf("thereinc\n");
        cLock->P();
        mutex->P();
        printf("consuming %d\n", i);
        mutex->V();
        pLock->V();
    //    lmutex->Acquire();
    //    //printf("tohere3 %d\n", buffer);
    //    while(buffer == 0) {
    //        lcLock->Acquire();
    //        lmutex->Release();
    //        lcCond->Wait(lcLock);
    //        lmutex->Acquire();
    //        lcLock->Release();
    //    }
    //    printf("consuming! %d products left\n", buffer);
    //    buffer--;
    //    lpLock->Acquire();
    //    lpCond->Signal(lpLock);
    //    lpLock->Release();
    //    lmutex->Release();
    //    currentThread->Yield();
    }
}

void ThreadTest4() {
    Thread *t1 = new Thread("producer thread");
    Thread *t2 = new Thread("consumer thread");
    //printf("here\n");
    t2->Fork(Consumer, 2);
    t1->Fork(Producer, 1);
}

void bthread(int arg) {
   printf("before barrier\n");
   barrier->setBarrier(arg);
   printf("after barrier\n");
}

void ThreadTest5() {
    Thread *t0 = new Thread("t50");
    Thread *t1 = new Thread("t51");
    Thread *t2 = new Thread("t52");
    barrier->initBarrier();

    t0->Fork(bthread, 0);
    t1->Fork(bthread, 1);
    t2->Fork(bthread, 2);
}

//----------------------------------------------------------------------
// ThreadTest
// 	Invoke a test routine.
//----------------------------------------------------------------------

void
ThreadTest()
{
    switch (testnum) {
    case 1:
	ThreadTest1();
	break;
    case 2:
    ThreadTest2();
    break;
    case 3:
    ThreadTest3();
    break;
    case 4:
    ThreadTest4();
    break;
    case 5:
    ThreadTest5();
    break;
    default:
	printf("No test specified.\n");
	break;
    }
}

