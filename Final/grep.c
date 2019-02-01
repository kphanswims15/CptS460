#include "ucode.c"

main(int argc, char *argv[])
{
  int in, out, n, outtty, i = 0, j = 0, length, flag;
  char buf[1024], pattern[128], output[500], tty[32];
  char *line1 = "$$$$$$$$$$$$$$$$$$$$";
  char *line2 = "Kimi's grep at work!";
  char *line3 = "$$$$$$$$$$$$$$$$$$$$";
  char *warning = "Not enough arguments\n";
  char *warning2 = "Cannot open file for grep\n";

  // get current tty port
  gettty(tty);

  // open up the current tty port for writing
  outtty = open(tty, O_WRONLY);

  write(outtty, line1, strlen(line1));
  write(outtty, "\n\r", 2);
  write(outtty, line2, strlen(line2));
  write(outtty, "\n\r", 2);
  write(outtty, line3, strlen(line3));
  write(outtty, "\n\r", 2);

  if (argc < 2)
  {
    write(outtty, warning, strlen(warning));
    return -1;
  }
  else if (argc == 2)
  {
    // using STDIN
    in = 0;
    out = 1;
    strcpy(pattern, argv[1]);
  }
  else
  {
    // using file
    in = open(argv[2], O_RDONLY);
    if (in < 0)
    {
      write(outtty, warning2, strlen(warning2));
      return -1;
    }
    strcpy(pattern, argv[1]);
    out = 1;
  }

  // check if the pattern is valid
  if (pattern[0] == 0)
  {
    return -1;
  }

  length = strlen(pattern);

  // loop to read the file byte from byte
  while(1)
  {
    // reads one by from the file or stdin
    n = read(in, buf, 1);

    // checks if there are still things to read
    if (n < 1)
    {
      return 0;
    }

    output[i] = buf[0];

    // handles the new line character
    if (buf[0] == '\n')
    {
      output[i++] = '\n';
      // if the pattern matches output print the line
      if (flag == 1)
      {
        // output it to the screen
        write(out, output, i);
        write(out, "\r", 1);
      }
      flag = 0;
      i = 0;
      j = 0;
      memset(output, 0, 500);
    }

    // go through the file line by line checking if the pattern is in the line
    if (output[i] == pattern[j])
    {
      j++;
      if (j == length)
      {
        flag = 1;
      }
    }
    else
    {
      j = 0;
    }
    i++;
  }
  close(in); close(out);
}
