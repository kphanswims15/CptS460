#include "ucode.c"

char tokens[16][32];
int numtokens, i;

int scan(char *line, char target)
{
  int pos = 0, length = 0;

  //start from endof string
  length = strlen(line);
  //printf("len = %d, line = %s\n", length, line);
  while(length > 0)
  {
    //printf("compare %c to %c\n", line[length], target);
    //search for target char
    if(line[length] == target)
    {
      return length;
    }
    length--;
  }
  return length;
}

int executeCmd(char *cmdLine)
{
  char head[64], tail[64];
  int out, in, fd;

  // check if there is io redirection
  out = scan(cmdLine, '>');
  in = scan(cmdLine, '<');

  if (in != 0)
  {
    // split the command into head and tail
    split(cmdLine, head, tail, in);
    // close fd 0
    close(0);

    // open the file for read only
    fd = open(tail, O_RDONLY);
    if (fd < 0)
    {
      printf("File %d does not exist\n", tail);
    }
    // execute the command
    exec(head);
  }

  if (out != 0)
  {
    // split the command into head and tail
    split(cmdLine, head, tail, out);

    // checks if you are appending the file
    out = scan(head, '>');
    if (out != 0)
    {
      // gets rid of the extra >
      head[out] = 0;
      // close fd 1
      close(1);

      // open the file for read only and append
      fd = open(tail, O_WRONLY | O_APPEND);
      if (fd < 0)
      {
        printf("File %s could not be opened\n", tail);
      }
      exec(head);
    }

    close(1);

    // if there is no appending then create the file to write to
    fd = open(tail, O_WRONLY | O_CREAT);
    if (fd < 0)
    {
      printf("File %s could not be opened\n", tail);
    }
    exec(head);
  }
  // If there is not redirection then execute the command
  exec(cmdLine);
}

int split(char *source, char *head, char *tail, int delimeterPos)
{
  int hIndex, tIndex, length, i = 0;

  length = strlen(source);

  // get the starting index of the tail to get the length
  tIndex = delimeterPos + 1;
  // get the ending index of the tail to get the length
  hIndex = delimeterPos - 1;

  // if there is not source string, head, or tail then it fails
  // split
  if (!source || !head || !tail)
  {
    return -1;
  }

  // set the tail index to before delimeter
  while (source[tIndex] && source[tIndex] == ' ')
  {
    tIndex++;
  }
  // set the head index to the beginning
  while (source[hIndex] && source[hIndex] == ' ')
  {
    hIndex--;
  }

  // copy the tail of the command into tail
  while (tIndex != length)
  {
    *tail = source[tIndex++];
    tail++;
  }
  *tail = 0;

  // copy the head of the command into head
  while (i <= hIndex)
  {
    *head = source[i++];
    head++;
  }
  *head = 0;
}

int do_pipe(char *cmdLine)
{
  int hasPipe = 0;
  char head[64], tail[64];
  int lpd[2], pid;

  for (int i = 0; i < strlen(cmdLine); ++i)
  {
    if (cmdLine[i] == '|')
    {
      hasPipe = i;
      break;
    }
  }

  // split the cmdLine into head and tail
  split(cmdLine, head, tail, hasPipe);


  // Note multiple pipes are handled recursively from right to left
  if(hasPipe)
  {
    // create the pipe: pd[0] = READ, pd[1] = WRITE
    pipe(lpd);

    // fork a child to share the pipe
    pid = fork();

    // parent: as pipe READER
    if (pid)
    {
      // close pipe WRITE end
      close(lpd[1]);
      // redirect stdin to pipe READ end
      dup2(lpd[0], 0);
      // recursion call on the tail
      do_pipe(tail);
    }
    // child : as pipe WRITER
    else
    {
      if (lpd)
      {
        // close pipe READ end
        close(lpd[0]);
        // redirect stdout to pipe write
        dup2(lpd[1], 1);
        // redirect stdout to pipe WRITE end
        executeCmd(head);
      }
    }
  }
  // pipe does not exsist
  else
  {
    executeCmd(cmdLine);
  }
}

int main(int argc, char *argv[])
{
  char cmdline[64], tty[16];
  int pid, status = 0;
  //NEVER DIE
  while(1)
  {
    // reset the commandline 
    memset(cmdline, 0, 64);
    while(cmdline[0] == 0)
    {
      printf("Enter command: ");
      gets(cmdline);
    }

    if(!strcmp(cmdline, "logout"))
    {
      exit(1);
    }

    // setup for child proc
    pid = fork();
    //parent process
    if(pid)
    {
      pid = wait(&status);
      printf("\nChild proccess %d, dies with status %d\n", pid, status);
      continue;
    }
    else
    {
      // go into do pipe
      do_pipe(cmdline);
    }
  }
}
