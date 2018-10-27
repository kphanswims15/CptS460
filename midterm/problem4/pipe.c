PIPE pipe[NPIPE];

int pipe_init()
{
  int i;
  PIPE *p;

  printf("pipe_init()\n");
  for (i = 0; i < NPIPE; i++)
  {
    p = &pipe[i];
    p->status = FREE;
  }
}

PIPE *create_pipe()
{
  for(int i = 0; i < NPIPE; i++)
  {
    if(pipe[i].status == FREE)
    {
      pipe[i].status = BUSY;
      pipe[i].head = 0;
      pipe[i].tail = 0;
      pipe[i].data = 0;
      pipe[i].room = PSIZE;
      return &pipe[i];
    }
  }
  return 0;
}

int read_pipe(PIPE *p, char *buf, int n)
{
  int r = 0;
  if(n <= 0)
    return 0;

  if (p->status != FREE)    // p->status must not be FREE
  {
    while(n)
    {
      while(p->data)
      {
        *buf++ = p->buf[p->tail++];   // read a byte to buf
        p->tail %= PSIZE;
        p->data--; p->room++; r++; n--;
        if (n==0)
          break;
      }
      kwakeup(&p->room);   // wakeup writers
      if (r)
        return r;
      ksleep(&p->data);    // sleep for data
    }
  }
}

int write_pipe(PIPE *p, char *buf, int n)
{
  int r = 0;
  if(n<=0)
    return 0;

  if (p->status != FREE)
  {
    while(n)
    {
      while(p->room)
      {
        p->buf[p->head++] = *buf++;
        p->head %= PSIZE;
        p->data++; p->room--; r++; n--;
        if (n==0)
          break;
      }
      kwakeup(&p->data);
      if (n==0)
        return r;
      ksleep(&p->room);
    }
  }
}
