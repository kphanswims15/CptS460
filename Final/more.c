
#include "ucode.c"

char buf[1024];

main(int argc, char *argv[])
{
  int in, out, outtty, intty, n, line = 0;
  char tty[32], c;

  char *line1 = "$$$$$$$$$$$$$$$$$$$$";
  char *line2 = "Kimi's more at work!";
  char *line3 = "$$$$$$$$$$$$$$$$$$$$";

  char *warning = "Unable to open the file\n";
  // get the current tty
  gettty(tty);

  // open the current tty for read and write
  intty = open(tty, O_RDONLY);
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
    // read one back at time from the file
    n = read(in, buf, 1);

    // check if there is stuff left in the file
    if (n < 1)
    {
      return 0;
    }

    // write the bytes one at a time
    write(outtty, buf, 1);

    // print the new line
    if (buf[0] == '\n')
    {
      write(outtty, "\r", 1);

      if (line < 25)
      {
        line++;
      }
      else
      {
        // wait for user input
        read(intty, &c, 1);

        // if the user pushes enter
        if(c == '\r')
        {
          line--;
        }
        // if the user presses space print whole page
        else if(c == ' ')
        {
          line = 0;
        }
      }
    }
  }
  close(in); close(outtty); close(intty);
}
