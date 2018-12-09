#include "ucode.c"

char buf[1024];

main(int argc, char *argv[])
{
  int in, out, outtty, inttty, n, line = 0;
  char ttty[32], c;

  char *line1 = "$$$$$$$$$$$$$$$$$$$$";
  char *line2 = "Kimi's more at work!";
  char *line3 = "$$$$$$$$$$$$$$$$$$$$";

  gettty(ttty);

  inttty = open(ttty, O_RDONLY) ;
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

    // check if there is stuff left in the file
    if (n < 1)
    {
      break;
    }

    /*if (in == 0)
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
    {*/
      // write the bytes one at a time
     write(outtty, buf, 1);

      // print the new line
      if (buf[0] == '\n')
      {
        write(outtty, "\r", 1);

        // prints a page
        if (line < 25)
        {
          line++;
        }
        else
        {
          read(inttty, &c, 1);
          // if the user presses space
          if(c == '\r')
          {
            line--;
          }
          else if(c == ' ')
          {
            line = 0;
          }
        }
      }
    //}
  }
  close(in); close(outtty);
}
