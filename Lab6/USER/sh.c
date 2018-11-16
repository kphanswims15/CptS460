#include "ucode.c"

int main(int argc, char *argv[])
{
  int pid, status;
  char cmdline[128];

  while(1)
  {
    // display executable commands in /bin directory
    // prompt for a command line cmdline = "cmd a1 a2 .... an"


    if(!strcmp(cmd, "exit"))
    {
      exit(0);
    }
    // fork a child process to execute the cmd line
    pid = fork();
    if(pid) // parent sh waits for child to die
      pid = wait(&status);
    else // child exec cmdline
      exec(cmdline);    // exec("cmd a1 a2 .... an")
  }
}
