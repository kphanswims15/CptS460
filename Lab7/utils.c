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

MINODE *iget(int dev, int ino, MINODE *minode)
{
  char buf[BLKSIZE];
  int  found = 0, block, disp;
  INODE *ip;
  MINODE *mip = minode;
  GD *gp;

  // Search for an item pointed to
  // search minode[] array for an item pointed by mip with the same (dev, ino)
  while (mip->dev < 0 || mip->refCount > 0)
  {
    if (mip->dev == dev && mip->ino)
    {
      found = 1;
      break;
    }
    mip++;
  }

  if (!found)
  {
    // Inode is not loaded into memory
    // searches minode array for mip with refCount = 0
    mip = minode;
    while (mip->dev < 0 || mip->refCount > 0)
    {
      mip++;
    }

    // mark it in use
    mip->refCount = 1;

    // Asign the value of dev and ino
    mip->dev = dev;
    mip->ino = ino;

    // Initialize other fields: dirty = 0, mounted = 0, mountPtr = 0
    mip->dirty = 0;
    mip->mounted = 0;
    mip->mptr = 0;

    // Assign to blk and disp
    get_block(mip->dev, 2, buf);
    gp = (GD *)buf;

    // block comtains the inode
    block = (mip->ino - 1) / 8 + gp->bg_inode_table;
    // which inode in the block
    disp = (mip->ino - 1) % 8;

    // Load the block into buf
    get_block(dev, block, buf);

    // Point ip to INODE in buf
    ip = ((INODE *)buf) + disp;

    // copy INODE into minode.INODE
    mip->INODE = *ip;
  }
  else
  {
    mip->refCount++;
    return mip;
  }
  return mip;
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
  get_block(fd, 1, buf);
  sp = (SUPER *)buf;

  // checks if it is a valid device
  if (sp->s_magic != 0xEF53)
  {
    printf("%s is NOT an EXT2 FS\n", devName);
    return 1;
  }
}
