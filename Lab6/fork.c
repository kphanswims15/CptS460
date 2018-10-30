int body(), goUmode();
char *istring = "init start";

PROC *kfork(char *filename)
{
  // called by P0 to creat P1 with /bin/init as Umode image
}

int fork()
{
  // fork a CHILD process as in Unix/Linux
  int i, PA, CA;
  PROC *p = getproc();

  if (p == 0)
  {
    printf("fork failed\n");
    return -1;
  }
  p->ppid = running->pid;
  p->parent = running;
  p->status = READY;
  p->priority = 1;

  printf("running usp = %x linkR = %x\n", running->usp, running->upc);

  PA = (running->pgdir[2048] & 0xFFFF0000); // parent Umode PA
  CA = (p->pgdir[2048] & 0xFFFF0000);       // child Umode PA

  printf("FORK: child %d uimage at %x\n", p->pid, CA);
  printf("copy Umode image from %x to %x\n", PA, CA);

  // copy 1MB of Umode image
  memcpy((char *)CA, (char *)PA, 0x100000);

  // both should be in their VA sections
  p->usp = running->usp;
  p->ucpsr = running->ucpsr;

  // the hard part: child must resume to the same place as the parent
  // child kstack must contain |parent kstack|goUmode stack| => copy kstack
  printf("copy kernel mode stack\n");

  //j = &running->kstack[SSIZE] - running->ksp;
  // printf("j=%d\n", j);

  //  this frame must be copied from parent's kstack, except PC, r0=0
  //  1  2  3  4   5  6  7  8  9  10 11 12 13 14
  // ----------------------------------------------
  //  PC ip fp r10 r9 r8 r7 r6 r5 r4 r3 r2 r1 r0  |
  //-----------------------------------------------
  //   15    16 17 18  19 20 21 22 23 24 25 26 27 28
  //-----------------------------------------------------
  // goUmode ip fp r10 r9 r8 r7 r6 r5 r4 r3 r2 r1 r0 |
  //  --------------------------------------------|ksp----
  for (i = 1; i <= 14; i++)
  {
    p->kstack[SSIZE - i] = running->kstack[SSIZE - i];
  }

  for (i = 15; i <=28; i++)
    p->kstack[SSIZE - i] = 0;

  // child return pid = 0
  p->kstack[SSIZE - 14] = 0;
  p->kstack[SSIZE - 15] = (int)goUmode;
  p->ksp = &(p->kstack[SSIZE - 28]);

  enqueue(&readyQueue, p);

  printf("KERNEL: proc %d forked a child %d\n", running->pid, p->pid);
  printQ(readyQueue);

  return p->pid;
}
