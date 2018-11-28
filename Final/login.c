/********** login **********/
// login.c: Upon entry, argv[0]=login, argv[1]=/dev/ttyx
#include "ucode.c"
#include <string.h>

int in, out, err, fd;
char name[128], password[128], line[64], buf[1024];

int tokenize(char *source, char *delimiter)
{
  char str_buff[64];
  char *token;
  int i = 0, total = 0;

  if (!source || !delimiter)
  {
    printf("bad inputs to tokenize\n");
    return -1;
  }

  // strtok destroys source

}

main(int argc, char *argv[])
{
    // 1. close file description 0, 1 inherited from INIT
  close(0);
  close(1);

  // 2. open argv[1] 3 times as in(0), out(1), err(2)
  in = open(argv[1], 0);
  out = open(argv[1], 1);
  err = open(argv[1], 2);

  // 3. settty(argv[1])
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

    read(fd, buf, 1024);
    getline(buf);
    // 6. if (user has a valid account)

        /* 7. change uid, gid to user's uid, gid // chuid()
              change cwd to user's home DIR // chdir()
              close opened /ect/passwd file // close()*/

       // 8. exec to program in user account // exec()
  }
  printf("login failed, try again\n");
}
