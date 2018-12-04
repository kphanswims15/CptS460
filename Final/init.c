/********** init.c file **********/
#include "ucode.c"

int console, ser0, ser1;

int parent() // P1's code
{
  int pid, status;
  while(1)
  {
    printf("INIT: wait for ZOMBIE child\n");
    pid = wait(&status);
    // if console login process died
    if(pid == console)
    {
      printf("INIT: forks a new console login\n");
      // fork another one
      console = fork();
      if(console)
        continue;
      else
        // new console login process
        login();
    }

    if(pid == ser0)
    {
      printf("INIT: forks a new login on serial0");
      // fork another one
      ser0 = fork();
      if(ser0)
        continue;
      else
        // new serial0 login process
        loginser0();
    }

    if (pid == ser1)
    {
      printf("INIT: forks a new login on serial1");
      // fork another one
      ser1 = fork();
      if(ser1)
        continue;
      else
        // new serial1 loging process
        loginser1();
    }

    printf("INIT: I just buried an orphan child proc %d\n", pid);
  }
}

int login()
{
  exec("login /dev/tty0");
}

int loginser0()
{
  exec("login /dev/ttyS0");
}

int loginser1()
{
  exec("login /dev/ttyS1");
}

main()
{
  // file descriptors for terminal I/O
  int in, out;

  // file descriptor 0
  in = open("/dev/tty0", O_RDONLY);
  // for display to console
  out = open("/dev/tty0", O_WRONLY);

  console = fork();

  if (console) {    // parent
    ser1 = fork();
    if (ser1) {
      ser0 = fork();
      if (ser0) {
        parent();
      }
      else {
        loginser0();
      }
    }
    else {
      loginser1();
    }
  }
  else {            // child: exec to login on tty0
    login();
  }
}
