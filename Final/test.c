/********** test.c file *************/
#include "ucode.c"
#include <string.h>
char buf[1024];
char a[50] = "hi";
char b[50];

int main(int argc, char *argv[ ])
{
  int i;
  int fd, n;
  int pid = getpid();
  printf("KCW: PROC %d running test program\n", pid);

  strcpy(a, b);
  printf("%s\n\r", b);

  printf("argc = %d\n", argc);
  for (i=0; i<argc; i++)
    printf("argv[%d] = %s\n", i, argv[i]);

  fd = open("f1", O_RDONLY);
  printf("fd = %d\n", fd);
  if (fd < 0)
    exit(1);

  n = read(fd, buf, 1024);
  //strtok(buf, " ");
  printf("n=%d buf=%s\n", n, buf);

  printf("exit\n");
}
