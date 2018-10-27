int P(struct semaphore *s)
{
  int SR = int_off();
  s->value--;
  if (s->value < 0)
  {
    block(s);
  }
  int_on(SR);
}

int V(struct semaphore *s)
{
  int SR = int_off();
  s->value++;
  if(s->value <= 0)
  {
    signal(s);
  }
  int_on(SR);
}
