/*********  t.c file *********************/

int prints(char *s)
{
  while(*s != '\0')
  {
    putc(*s);
    s++;
  }
  return 1;
}

int gets(char *s)
{
  while((*s = getc()) != '\r')
  {
    putc(*s++);
  }
  *s = 0;
  return 1;
}

char ans[64];

main()
{
  while(1){
    prints("What's your name? ");
    gets(ans);  prints("\n\r");

    if (ans[0]==0){
      prints("return to assembly and hang\n\r");
      return;
    }
    prints("Welcome "); prints(ans); prints("\n\r");
  }
}
