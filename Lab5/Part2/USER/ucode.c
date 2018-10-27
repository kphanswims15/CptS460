typedef unsigned char   u8;
typedef unsigned short  u16;
typedef unsigned int    u32;

#include "string.c"
#include "uio.c"

int ubody(char *name)
{
  int pid, ppid;
  char line[64];
  u32 mode,  *up;
  mode = getcsr();
  mode = mode & 0x1F;
  //printf("CPU mode=%x\n", mode);
  pid = getpid();
  ppid = getppid();
  /*********
  if (pid < 2){
    printf("test memory protection? [y|n]: ");
    ugetline(line); printf("\n");
    if (line[0]=='y'){
       printf("try 0x80200000 : ");
       up = (u32 *)0x80200000;
       *up = 123;
       printf("try 0x1000 : ");
       up = (u32 *)0x1000;
       *up=456;
    }
  }
  *********/
  while(1){
    if (strcmp(name, "one") == 0)
      printf("This is process #%d in Umode at %x parent=%d\n", pid, getPA(),ppid);
    else if (strcmp(name, "two") == 0)
      printf("THIS IS PROCESS #%d IN UMODE AT %x PARENT=%d\n", pid, getPA(), ppid);
    else if (strcmp(name, "three") == 0)
      printf("ThIs Is PrOcEsS #%d iN uMoDe At %x PaReNt=%d\n", pid, getPA(), ppid);
    else if (strcmp(name, "four") == 0)
      printf("tHiS iS pRoCeSs #%d In UmOdE aT %x pArEnT=%d\n", pid, getPA(), ppid);

    umenu();

    if (strcmp(name, "one") == 0)
      printf("input a command : ");
    else if (strcmp(name, "two") == 0)
      printf("INPUT A COMMAND : ");
    else if (strcmp(name, "three") == 0)
      printf("InPuT a CoMmAnD : ");
    else if (strcmp(name, "four") == 0)
      printf("iNpUt A cOmMaNd : ");

    ugetline(line);
    uprintf("\n");

    if (strcmp(line, "getpid")==0)
       ugetpid();
    if (strcmp(line, "getppid")==0)
       ugetppid();
    if (strcmp(line, "ps")==0)
       ups();
    if (strcmp(line, "chname")==0)
       uchname();
    if (strcmp(line, "switch")==0)
       uswitch();
    if (strcmp(line, "sleep")==0)
       usleep();
    if (strcmp(line, "wakeup")==0)
       uwakeup();
    if (strcmp(line, "getname")==0)
       ugetname();
  }
}

int umenu()
{
  uprintf("----------------------------------------------------\n");
  uprintf("getpid getppid ps chname switch sleep wakeup getname\n");
  uprintf("----------------------------------------------------\n");
}

int ugetname()
{
  char b[64];
  syscall(9,b,0,0);
  printf("name = %s\n", b);
}

int getpid()
{
  int pid;
  pid = syscall(0,0,0,0);
  return pid;
}

int getppid()
{
  return syscall(1,0,0,0);
}

int ugetpid()
{
  int pid = getpid();
  uprintf("pid = %d\n", pid);
}

int ugetppid()
{
  int ppid = getppid();
  uprintf("ppid = %d\n", ppid);
}

int ups()
{
  return syscall(2,0,0,0);
}
int uchname()
{
  char s[32];
  uprintf("input a name string : ");
  ugetline(s);
  printf("\n");
  return syscall(3,s,0,0);
}
int uswitch()
{
  return syscall(4,0,0,0);
}

int usleep()
{
  int pid = getpid();
  printf("proc %d go to sleep in kernel\n", pid);
  return syscall(5,pid,0,0);
}

int uwakeup()
{
  int pid;
  printf("enter a pid to wakeup: ");
  pid = geti();
  printf("\npid=%d\n", pid);
  return syscall(6,pid,0,0);
}

int ugetc()
{
  return syscall(90,0,0,0);
}

int uputc(char c)
{
  return syscall(91,c,0,0);
}
int getPA()
{
  return syscall(92,0,0,0);
}
int argc;
char *argv[32];

int token(char *line)
{
  int i;
  char *cp;
  cp = line;
  argc = 0;

  while (*cp != 0){
       while (*cp == ' ') *cp++ = 0;
       if (*cp != 0)
           argv[argc++] = cp;
       while (*cp != ' ' && *cp != 0) cp++;
       if (*cp != 0)
           *cp = 0;
       else
            break;
       cp++;
  }
  argv[argc] = 0;
}
