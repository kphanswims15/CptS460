#include "ucode.c"

char *t1 = "xwrxwrxwr-------";
char *t2 = "----------------";

// list a single file
int ls_file(char *fname)
{
  struct stat fstat, *sp = &fstat;

  int r, i;
  char sbuf[4096];

  // stat the file
  r = stat(fname, sp);

  // print d if the file is a directory
  if (sp->st_mode == 0x41ed)
  {
    prints("d");
  }
  else
  {
    // print - if the file is reg
    prints("-");
  }

  // bit permissions
  for (i = 8; i >= 0; i--)
  {
    // print permission bit as r w x
    if (sp->st_mode & (1 << i))
      mputc(t1[i]);
    else
      // print permission as -
      mputc(t2[i]);
  }

  // link count
  printf(" %d ", sp->st_nlink);
  // uid
  printf(" %d", sp->st_uid);
  // gid
  printf(" %d", sp->st_gid);
  // filesize
  printf(" %d", sp->st_size);

  // file name
  printf(" %s\n\r", fname);
}

main(int argc, char *argv[])
{
  struct stat* sbuf, mystat;
  int n, r;
  int fd;

  char cwd[32], buf2[1024], name[128];

  // get the current working directory
  getcwd(cwd);

  if (argc > 1)
  {
    // ls a dir
    fd = open(argv[1], O_RDONLY);
    strcpy(cwd, argv[1]);
  }
  else
  {
    // no parameter so ls the current directory
    fd = open(cwd, O_RDONLY);
  }

  chdir(cwd);

  if (fd < 0)
  {
    printf("File not opened successfully\n");
    close(fd);
    return 0;
  }

  // read the current directory
  n = read(fd, buf2, 1024);
  char* cp = buf2;
  DIR* dp = (DIR*)cp;

  // go through the directory
  while (cp < buf2 + 1024)
  {
    // ls each entry with permission bits
    strcpy(name, dp->name);
    strcat(name, "\0");
    ls_file(name);

    cp += dp->rec_len;
    dp = (DIR*)cp;
  }

  close(fd);
}
