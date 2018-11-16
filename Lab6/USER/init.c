#include "ucode.c"

int main(int argc, char *argv[])
{
  int sh, pid, status;

  sh = fork();
  if (sh)     // P1 runs in a while(1) loop
  {
    while(1)
    {
      pid = wait(&status); // wait for ANY child to die
      if (pid == sh)      // if sh died, fork another one
      {
        sh = fork();
        continue;
      }
      printf("P1: I just buried an orphan %d\n", pid);
    }
  }
  else
    exec("sh");   // child of P1 runs sh
}
