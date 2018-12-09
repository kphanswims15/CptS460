#include "ucode.c"

main(int argc, char *argv[])
{
  int in, out, outtty, n;
  char buf[1024], ttty[32], c;
  char *line1 = "$$$$$$$$$$$$$$$$$$$$$$$$$";
  char *line2 = "Kimi's cat at work! Meow!";
  char *line3 = "$$$$$$$$$$$$$$$$$$$$$$$$$";

  gettty(ttty);

  outtty = open(ttty, O_WRONLY);

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
    out = 1;
  }

  // loop to read the file byte from byte
  while(1)
  {
    n = read(in, buf, 1);

    if (n < 1)
    {
      break;
    }

    if (in == 0)
    {
      if (buf[0] != 13)
      {
        write(out, buf, 1);
      }
      else
      {
        write(out, "\n\r", 2);
      }
    }
    else
    {
      write(out, buf, 1);
      if (buf[0] == '\n')
      {
        write(out, "\r", 1);
      }
    }
  }
  close(in); close(outtty);
}
