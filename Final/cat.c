#include "ucode.c"

main(int argc, char *argv[])
{
  int in, out, n;
  char buf[1024];

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

  prints("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
  prints("Kimi's cat running! MEOW!!!!!\n");
  prints("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

  // loop to read the file byte from byte
  while(1)
  {
    n = read(in, buf, 1);
    if (n < 1)
    {
      break;
    }

    if (buf[0] == '\n')
    {
      prints("\n\r");
    }
    // if the user enters the enter key
    else if (buf[0] == 13)
    {
      prints("\n\r");
    }
    else
    {
      write(out, buf, 1);
    }
  }
  close(in);
}
