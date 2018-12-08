#include "ucode.c"

main(int argc, char *argv[])
{
  int in, out, n, line = 0;
  char buf[1024], c;

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

  prints("$$$$$$$$$$$$$$$$$$$\n");
  prints("Kimi's more running\n");
  prints("$$$$$$$$$$$$$$$$$$$\n");

  // loop to read the file byte from byte
  while(1)
  {
    n = read(in, buf, 1);

    // check if there is stuff left in the file
    if (n < 1)
    {
      break;
    }

    // print the new line
    if (buf[0] == '\n')
    {
      prints("\n\r");

      // prints a page
      if (line < 25)
      {
        line++;
      }
      else
      {
        c = getc();
        // if the user presses space
        if (c == ' ')
        {
          // line to 0
          line = 0;
        }
        // if the user presses enter
        else if (c == 13)
        {
          continue;
        }
      }
    }
    // if the user enters the enter key
    else if (buf[0] == 13)
    {
      prints("\n\r");
    }
    else
    {
      // write the bytes one at a time
      write(out, buf, 1);
    }
  }
  close(in);
}
