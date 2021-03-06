#include "ucode.c"

main(int argc, char *argv[])
{
  int in, out, outtty, i = 0, n;
  char buf[1024], tty[32], c;
  char string[128];
  char *line1 = "$$$$$$$$$$$$$$$$$$$$$$$$$";
  char *line2 = "Kimi's cat at work! Meow!";
  char *line3 = "$$$$$$$$$$$$$$$$$$$$$$$$$";
  char *warning = "Cannot open up file for cat\n\r";

  // get the current tty port
  gettty(tty);

  // open up the current tty port for writing
  outtty = open(tty, O_WRONLY);

  write(outtty, line1, strlen(line1));
  write(outtty, "\n\r", 2);
  write(outtty, line2, strlen(line2));
  write(outtty, "\n\r", 2);
  write(outtty, line3, strlen(line3));
  write(outtty, "\n\r", 2);

  if (argc == 1)
  {
    // using STDIN
    in = 0;
    out = 1;
  }
  else
  {
    // using file
    in = open(argv[1], O_RDONLY);
    if (in < 0)
    {
      write(outtty, warning, strlen(warning));
      return -1;
    }
    out = 1;
  }

  // loop to read the file byte from byte
  while(1)
  {
    // reads one byte at a time from the file
    n = read(in, buf, 1);

    // Checks if there is still bytes left in the file to read
    if (n < 1)
    {
      return 0;
    }

    // uses STDIN
    if (in == 0)
    {
      line[i] = buf[0];
      if (buf[0] != 13)
      {
        // write byte by byte out to stdinout
        write(out, buf, 1);
        i++;
      }
      else
      {
        // handles the instance if the user presses enter
        write(out, "\n\r", 2);
        // echo the line out that was put in by stdin
        write(outtty, line, i);
        write(outtty, "\n\r", 2);
        memset(line, 0, 128);
        i = 0;
      }
    }
    else
    {
      // writes byte by byte out to stdinout
      write(out, buf, 1);
      // handles the newline character
      if (buf[0] == '\n')
      {
        write(outtty, "\r", 1);
      }
    }
  }

  close(in); close(outtty);
}
