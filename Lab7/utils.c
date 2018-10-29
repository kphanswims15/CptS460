#include "utils.h"

void init(PROC *p[], MINODE *minode[], int numMInodes, MINODE **root)
{
  int i = 0;

  // allocating memory for the processes
  *p = (PROC *)(malloc(2 * sizeof(PROC)));

  // setting the uids for each process
  (*p)[0].uid = 0;
  (*p)[0].gid = 0;

  // get the uid and the gid of the user
  (*p)[1].geteuid();
  (*p)[1].getegid();

  // setting the cwd of each process to 0
  (*p)[0].cwd = NULL;
  (*p)[1].cwd = NULL;

  // setting all of the refCount of the minode to zero
  *minode = (MINODE *)malloc((numMInodes + 1) * sizeof(MINODE));

  for (i = 0; i < numMInodes; i++)
  {
    (*minode)[i].refCount = 0;
  }

  // User dev = -1 to denote end of array
  (*minode)[numMInodes].dev = -1;

  // setting root to NULL
  *root = NULL;
}

int get_block(int fd, int blk, char *buf)
{
  lseek(fd, (long)blk * BLKSIZE, SEEK_SET);
  return read(fd, buf, BLKSIZE);
}

int mount_root(char *devName, MINODE **root, MINODE *minode, PROC **running, PROC *p[], struct mntTable **mtables)
{
  char buf[BLKSIZE];
  int fd = 0;
  SUPER *sp;
  GD *gd;
  struct mntTable *mtable;

  // opening the device for read and write minode
  fd = open(devName, O_RDWR);
  if (fd < 0)
  {
    printf("Open %s failed\n", devName);
    return 1;
  }

  // reads the SUPER block
}
