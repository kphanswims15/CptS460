#include "ucode.c"

void toUpper(char *s)
{
    int i=0;
    while(s[i]!='\0')
    {
        if(s[i]>='a' && s[i]<='z'){
            s[i]=s[i]-32;
        }
        ++i;
    }
}

main(int argc, char *argv[])
{
  int in, out, outtty, n;
  char buf[1024], ttty[32], c;
  char string[128];
  char *line1 = "$$$$$$$$$$$$$$$$$$$";
  char *line2 = "Kimi's l2u at work!";
  char *line3 = "$$$$$$$$$$$$$$$$$$$";
  char *warning = "Unable to open up files for l2u\n";

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
  else if (argc == 3)
  {
    // using files
    // open file 1 for read
    in = open(argv[1], O_RDONLY);
    // open file 2 for write
    out = open(argv[2], O_WRONLY | O_CREAT);

    if (in < 0 && out < 0)
    {
      write(outtty, warning, strlen(warning));
      return -1;
    }
  }

  // loop to read the file byte from byte
  while(1)
  {
    n = read(in, buf, 1);

    if (n < 1)
    {
      break;
    }

    // uses STDIN
    if (in == 0)
    {
      if (buf[0] != 13)
      {
        // write byte by byte out to stdinout
        toUpper(buf);
        write(out, buf, 1);
      }
      else
      {
        // handles the instance if the user presses enter
        write(out, "\n\r", 2);
        memset(line, 0, 128);
      }
    }
    else
    {
      toUpper(buf);
      write(out, buf, 1);
      if (buf[0] == '\n')
      {
        write(outtty, "\r", 1);
      }
    }
  }
  close(in); close(outtty); close(out);
}
