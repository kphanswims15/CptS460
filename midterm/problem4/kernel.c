/********************************************************************
Copyright 2010-2017 K.C. Wang, <kwang@eecs.wsu.edu>
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/

/********************
#define  SSIZE 1024
#define  NPROC  9
#define  FREE   0
#define  READY  1
#define  SLEEP  2
#define  BLOCK  3
#define  ZOMBIE 4
#define  printf  kprintf

typedef struct proc{
  struct proc *next;
  int    *ksp;
  int    status;
  int    pid;

  int    priority;
  int    ppid;
  struct proc *parent;
  int    event;
  int    exitCode;
  int    kstack[SSIZE];
}PROC;
***************************/
#define NPROC 9
PROC proc[NPROC], *running, *freeList, *readyQueue;
int procsize = sizeof(PROC);
int body();

int atoi(char *s)
{
  int v = 0;
  while(*s)
  {
    v = v * 10 + (*s - '0');
    s++;
  }
  return v;
}

int geti()
{
  char s[16];
  kgets(s);
  return atoi(s);
}

int init()
{
  int i, j;
  PROC *p;
  kprintf("kernel_init()\n");
  for (i=0; i<NPROC; i++){
    p = &proc[i];
    p->pid = i;
    p->status = READY;
    p->next = p + 1;
  }
  proc[NPROC-1].next = 0; // circular proc list
  freeList = &proc[0];
  readyQueue = 0;

  printf("create P0 as initial running process\n");
  p = dequeue(&freeList);
  p->priority = 0;
  p->ppid = 0; p->parent = p;  // P0's parent is itself
  running = p;
  kprintf("running = %d\n", running->pid);
  printList("freeList", freeList);
}

int ksleep(int event)
{
  int SR = int_off(); // disable IRQ and return CPSR
  running->event = event;
  running->status = SLEEP;
  tswitch();
  int_on(SR);
}

int kwakeup(int event)
{
  int i = 0;
  PROC *p;

  int SR = int_off(); // disable IRQ and return CPSR

  for (i = 1; i < NPROC; i++)
  {
    p = &proc[i];
    if (p->status == SLEEP && p->event == event)
    {
      p->status == READY;
      enqueue(&readyQueue, p);
    }
  }
  int_on(SR);
}

void kexit(int exitValue)
{
  int i;
  PROC *p;
  /*printf("proc %d kexit\n", running->pid);
  running->status = FREE;
  running->priority = 0;
  enqueue(&freeList, running);   // putproc(running);*/

  if (running->pid == 1)
  {
    printf("Cannot kill process 1\n");
    return;
  }

  for (i = 0; i < NPROC; i++)
  {
    p = &proc[i];

    // sending children to the orphanage
    if(p->status != FREE && p->ppid == running->pid)
    {
      p->ppid = 1;
      p->parent = &proc[1];
    }
  }

  running->exitCode = exitValue;
  running->status = ZOMBIE;

  kwakeup(&proc[1]);
  kwakeup(running->parent);

  tswitch();
}

PROC *kfork(int func, int priority)
{
  int i;
  PROC *p = dequeue(&freeList);
  if (p==0){
    printf("no more PROC, kfork failed\n");
    return 0;
  }
  p->status = READY;
  p->priority = priority;
  p->ppid = running->pid;
  p->parent = running;

  if(p->parent->child == 0)
  {
    p->parent->child = p;
  }
  else
  {
    PROC *temp = p->parent->child;

    while(temp->sibling != 0)
    {
      temp = temp->sibling;
    }

    temp->sibling = p;
  }

  printf("\nrunning pid: %d\n", running->pid);
  printf("running ppid: %d\n", running->ppid);

  PROC *temp = running->child;
  if (running->child != 0)
  {
    printf("running children pid: %d\n", running->child->pid);
  }

  while(temp->sibling != 0)
  {
    printf("running children pid: %d\n", temp->sibling->pid);
    temp = temp->sibling;
  }

  // set kstack to resume to body
  // stack = r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r14
  //         1  2  3  4  5  6  7  8  9  10 11  12  13  14
  for (i=1; i<15; i++)
      p->kstack[SSIZE-i] = 0;
  p->kstack[SSIZE-1] = (int)func;  // in dec reg=address ORDER !!!
  p->ksp = &(p->kstack[SSIZE-14]);
  enqueue(&readyQueue, p);
  printf("%d kforked a child %d\n", running->pid, p->pid);
  printList("readyQueue", readyQueue);
  return p;
}

int kwait(int *status)
{
  PROC *p;

  if (running->child == 0)
  {
    return -1;
  }

  while(1)
  {
    // if can find a ZOMBIE child
    if (running->child->status == ZOMBIE)
    {
      // copy ZOMBIE child's exitCode to *status
      *status = running->child->exitCode;
      // delete ZOMBIE child from children list
      running->child->status = FREE;
      // release the ZOMBIE child PROC as FREE to freeList
      enqueue(&freeList, running);
      // returns ZOMBIE child pid
      return running->child->pid;
    }
    else
    {
      // check the siblings to see if they are zombies
      PROC *p = running->child->sibling;
      while(p != 0)
      {
        if (p->status == ZOMBIE)
        {
          // copy ZOMBIE child's exitCode to *status
          *status = p->exitCode;
          // delete ZOMBIE child from children list
          p->status = FREE;
          // release the ZOMBIE child PROC as FREE to freeList
          enqueue(&freeList, p);
          // returns ZOMBIE child pid
          return p->pid;
        }
        p = p->sibling;
      }
    }
    ksleep(running);
  }
}

int killpipe(int event)
{
  int SR = int_off();

  running->event = event;
  running->status = SLEEP;

  int_on(SR);
}

int scheduler()
{
  kprintf("proc %d in scheduler ", running->pid);
  if (running->status == READY)
      enqueue(&readyQueue, running);
  running = dequeue(&readyQueue);
  kprintf("next running = %d\n", running->pid);
}

int do_exit()
{
  int exitValue;
  printf("Enter an exitValue: ");
  exitValue = geti();
  printf("%d\n", exitValue);
  kexit(exitValue);
}

int do_wait()
{
  int status;
  int pid = kwait(&status);
  printf("\nZombie pid: %d\n", pid);
  printf("Zombie exitCode: %d\n", status);
}

int body(int pid, int ppid, int func, int priority)
{
  char c; char line[64];
  int exitValue, status;

  //int pid;
  printf("\npid: %d\n", pid);
  printf("ppid: %d\n", ppid);
  printf("func: %x\n", func);
  printf("priority: %d\n", priority);

  kprintf("proc %d resume to body()\n", running->pid);
  while(1){
    pid = running->pid;
    if (pid==0) color=BLUE;
    if (pid==1) color=WHITE;
    if (pid==2) color=GREEN;
    if (pid==3) color=CYAN;
    if (pid==4) color=YELLOW;
    if (pid==5) color=WHITE;
    if (pid==6) color=GREEN;
    if (pid==7) color=WHITE;
    if (pid==8) color=CYAN;

    printList("readyQueue", readyQueue);
    printList("freeList", freeList);
    kprintf("proc %d running, parent = %d\n", running->pid, running->ppid);
    kprintf("input a char [s|f|q|w|r] : ");
    c = kgetc();
    printf("%c\n", c);

    switch(c){
      case 's': tswitch();                break;
      case 'f': kfork((int)body, 1);      break;
      case 'q': do_exit();                break;
    }
  }
}
