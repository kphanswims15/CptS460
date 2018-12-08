/********** login **********/
// login.c: Upon entry, argv[0]=login, argv[1]=/dev/ttyx
#include "ucode.c"

char buf[1024];

int parser(char *line, char *names[ ], char token)
{
  int i, n; char *cp;

  n = 0;
  for (i=0; i<16; i++)
      names[i]=0;

  cp = line;
  while (*cp != 0){
       while (*cp == ' ') // skip over blanks
              *cp++ = 0;
       if (*cp != 0)
           names[n++] = cp;
       while (*cp != token && *cp != 0) // scan over token chars
	       cp++;
       if (*cp != 0)
	   *cp = 0;
       else
           break;
       cp++;
  }

  return n;
}

main(int argc, char *argv[])
{
  int i = 0;
  int in, out, err;
  int fd, n, size;
  char *elements[100], *lines[100], *yo[100];
  char name[128], password[128];

  // 1. close file descriptors 0,1 inherited from INIT
  close(0);
  close(1);

  // 2. open argv[1] 3 times as in(0), out(1), err(2)
  in =  open(argv[1], 0);
  out = open(argv[1], 1);
  err = open(argv[1], 2);

  // 3. settty(argv[1]); // set tty name string in PROC.tty
  settty(argv[1]);

  // 4. open /etc/passwd file for READ
  fd = open("/etc/passwd", O_RDONLY);

  while(1)
  {
    /* 5 printf("login:") gets(name)
       printf("password:") gets(password);
       for each line in /etc/passwd file do{
       tokenize user account line; */
    printf("login:");
    gets(name);

    printf("password:");
    gets(password);

    n = read(fd, buf, 1024);
    if (n <= 0)
    {
      printf("There are no passwords found\n");
    }

    size = parser(buf, lines, '\n');

    for (i = 0; i < size; i++)
    {
      parser(lines[i], elements, ':');

      // 6. if (user has a valid account)
      // username:password:uid:gid:mode:directory:execmd
      if (strcmp(name, elements[0]) == 0 && strcmp(password, elements[1]) == 0)
      {
        /* 7. change uid, gid to user's uid, gid // chuid()
              change cwd to user's home DIR // chdir()
              close opened /ect/passwd file // close()*/
        chuid(atoi(elements[2]), atoi(elements[3]));
        chdir(elements[5]);
        close(fd);

        // 8. exec to program in user account // exec()
        printf("KPLOGIN: Welcome! %s\n", elements[0]);
        printf("KPLOGIN: cd to HOME=%s change uid to %d\n", elements[5], atoi(elements[2]));
        exec(elements[6]);
        return 1;
      }
    }
  }
  printf("login failed, try again\n");
}
