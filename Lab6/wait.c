int ksleep(int event)
{
  printf("proc %d ksleep on %x\n", running->pid, event);
  running->event = event;
  running->status = SLEEP;
  enqueue(&sleepList, running);
  printf("sleepList = ");
  printQ(sleepList);
  tswitch();
}

int kwakeup(int event)
{
  PROC *p, *tmp = 0;
  while((p = dequeue(&sleepList)) != 0)
  {
    if (p->event == event)
    {
      printf("kwakeup %d\n", p->pid);
      p->status = READY;
      enqueue(&readyQueue, p);
    }
    else
    {
      enqueue(&tmp, p);
    }
  }
  sleepList = tmp;
}

int kexit(int value)
{
}

int kwait(int *status)
{
}
