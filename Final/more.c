#include "ucode.c"

main(int argc, char *argv[])
{
  int in, out, n, line = 0;
  char buf[1024], c;

  if (argc == 1)
  {
    in = 0;
    out = 1;
  }
  else
  {
    // catting a file
    in = open(argv[1], O_RDONLY);
    out = 1;
  }

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

      if (line < 25)
      {
        line++;
      }
      else
      {
        c = getc();
        if (c == ' ')
        {
          line = 0;
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
