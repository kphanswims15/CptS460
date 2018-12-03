/********** test.c file *************/
#include "ucode.c"

char buf[1024];

int eat(char *line, char *name[ ])
{
  int i, n; char *cp;

  n = 0;
  for (i=0; i<16; i++)
      name[i]=0;

  cp = line;
  while (*cp != 0){
       while (*cp == ' ') // skip over blanks
              *cp++ = 0;
       if (*cp != 0)
           name[n++] = cp;
       while (*cp != ':' && *cp != 0) // scan over token chars
	       cp++;
       if (*cp != 0)
	   *cp = 0;
       else
           break;
       cp++;
  }

  for (i=0; i < n; i++){
      if (name[i]){
         prints(name[i]); prints("  ");
      }
  }
  prints("\n\r");

  return n;
}

int eatfile(char *line, char *name[ ])
{
  int i, n; char *cp;

  n = 0;
  for (i=0; i<16; i++)
      name[i]=0;

  cp = line;
  while (*cp != 0){
       while (*cp == ' ') // skip over blanks
              *cp++ = 0;
       if (*cp != 0)
           name[n++] = cp;
       while (*cp != '\n' && *cp != 0) // scan over token chars
	       cp++;
       if (*cp != 0)
	   *cp = 0;
       else
           break;
       cp++;
  }

  for (i=0; i < n; i++){
      if (name[i]){
         prints(name[i]); prints("  ");
      }
  }
  prints("\n\r");

  return n;
}


int main(int argc, char *argv[ ])
{
  int i;
  int fd, n;
  char *name[100];
  char *something;
  int pid = getpid();
  printf("KCW: PROC %d running test program\n", pid);

  something = "root:12345:1000:0:superuser:/root:sh\nhello:54321:2000:0:user:/user:sh";

  eatfile(something, name);

  printf("argc = %d\n", argc);
  for (i=0; i<argc; i++)
    printf("argv[%d] = %s\n", i, argv[i]);

  fd = open("f1", O_RDONLY);
  printf("fd = %d\n", fd);
  if (fd < 0)
    exit(1);

  n = read(fd, buf, 1024);

  printf("n=%d buf=%s\n", n, buf);

  printf("exit\n");
}
