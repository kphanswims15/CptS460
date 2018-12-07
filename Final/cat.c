#include "ucode.c"

main(int argc, char *argv[])
{
  int in, out, n;
  char buf[1024], tty[128];

  in = open(argv[1], O_RDONLY);
  out = 1;

  while(n = read(in, buf, 1024))
  {
    buf[n] = 0;
    prints(buf);
  }
}
