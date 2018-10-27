#define MAX 8

typedef struct TimerNode {
  struct TimerNode *next;
  int seconds;      // the number of sec it waits for
  int pid;          // Proc pid
  int status;
} TIMERNODE;

TIMERNODE tL[MAX];
TIMERNODE *timerList;
TIMERNODE *freeTimers;

int countdown_init()
{
  int i;
  TIMERNODE *t;

  for (i=0; i<MAX; i++){
    t = &tL[i];
    t->seconds = 0;
    t->pid = 0;
    t->status = READY;
    t->next = t + 1;
  }
  tL[MAX-1].next = 0; // circular timerlist list
  freeTimers = &tL[0];
  timerList = 0;
  printTimerList("free", &freeTimers);
}

int enqueueTimer(TIMERNODE **queue, TIMERNODE *t)
{
  TIMERNODE *qp = *queue;

  // the queue is empty
  if (!qp)
  {
    t->next = qp;
    *queue = t;
    return 1;
  }

  // look at the front of the queue
  if (t->seconds < qp->seconds)
  {
    t->next = qp;
    *queue = t;
    return 1;
  }

  while(qp->next != 0 && t->seconds <= (qp->next)->seconds)
  {
    qp = qp->next;
  }

  t->next = qp->next;
  qp->next = t;
  return 1;
}

TIMERNODE *dequeueTimer(TIMERNODE **queue)
{
  TIMERNODE *tt;

  if(!tt)
  {
    return 0;
  }

  *queue = (*queue)->next;

  return tt;
}

int printTimerList(char *name, TIMERNODE *t)
{
  kprintf("%s = ", name);
  while(t){
    kprintf("[%d%d]->", t->seconds, t->pid);
    t = t->next;
 }
 kprintf("NULL\n");
}

void countdown_handler()
{
  countdown_init();

  TIMERNODE *timer, *timer2;

  timer = dequeueTimer(&freeTimers);
  timer->seconds = 3;
  timer->pid = 4;
  timer->status = READY;
  enqueueTimer(&timerList, timer);

  timer2->seconds = 5;
  timer2->pid = 7;
  timer2->status = READY;
  enqueueTimer(&timerList, timer2);
  printTimerList("timer: ", &timerList);

  printf("t: %d\n", &timerList[1].seconds);
  printTimerList("timer: ", &timerList);
}

/*int countdown_init()
  for (int i = 0; i < MAX; i++)
  {
    tL[i].seconds = 0;
    tL[i].pid = 0;
    tL[i].status = READY;
  }
}

int binSearch(TIMERNODE tList[], int size, TIMERNODE t)
{
  int low = 0, high = size = -1;

  if (t.value > tList[high])
    return size;

  while (high > low)
}

int countDown_insert(int value)
{

}*/
