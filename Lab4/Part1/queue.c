// queue.c file
extern PROC *freeList;
// WRITE YOUR OWN functions:

int enqueue(PROC **queue, PROC *p)
{
  int SR = int_off();  // IRQ interrupts off, return CPSR

  // grab the front of the queue
  PROC *qp = *queue;

  // place node in if the queue is empty
  if(!qp)
  {
    p->next = qp;
    *queue = p;
    int_on(SR);
    return 1;
  }

  // start at the front of the queue
  if(p->priority > qp->priority)
  {
    // put the node at the top of queue
    p->next = qp;
    *queue = p;
    int_on(SR);
    return 1;
  }

  // find a place to insert the node
  while(qp->next && p->priority <= (qp->next)->priority)
  {
      qp = qp->next;
  }

  // place the node after qp
  p->next = qp->next;
  qp->next = p;

  int_on(SR);          //  restore CPSR
  return 1;
}

PROC *dequeue(PROC **queue)
{
  int SR = int_off();  // IRQ interrupts off, return CPSR

  // grab the front of the queue
  PROC *pp = *queue;

  // checks if the queue is empty
  if(!pp)
  {
    return 0;
  }

  // go through the queue
  *queue = (*queue)->next;

  int_on(SR);          //  restore CPSR
  return pp;
}

int printList(char *name, PROC *p)
{
   kprintf("%s = ", name);
   while(p){
     kprintf("[%d%d]->", p->pid, p->priority);
     p = p->next;
  }
  kprintf("NULL\n");
}
