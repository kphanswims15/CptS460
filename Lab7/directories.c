#include "directories.h"

int chdirec(char *pathname, PROC *running, MINODE *root, MINODE *minode)
{
  MINODE *mip;
  int ino;

  if (pathname == NULL)
  {
    return 0;
  }

  if (pathname[0] == '\0')
  {
    // goes to root if there is no pathname
    running->cwd = iget(root->dev, 2, minode);
    return 1;
  }
  else
  {
    // cd to the pathname
    ino = getino(pathname, root, running, minode);
    mip = iget(root->dev, ino, minode);

    // checking if it is a directory
    if ((mip->INODE.i_mode & 0xF000) == 0x4000)
    {
      iput(running->cwd, minode);
      running->cwd = mip;
      return 1;
    }
    else
    {
      printf("%s is not a directory\n", pathname);
      return -1;
    }
  }
}

void rpwd(MINODE *cwd, MINODE *root, MINODE *minode)
{
  int i, ino, pino, maxBytes = 0, readBytes = 0, i_blocks[15];
  char *cp = NULL, buf[BLOCK_SIZE], filename[MAX];
  MINODE *pInode = NULL;
  DIR *dir = NULL;

  if (cwd->dev == root->dev && cwd->ino == root->ino) return;

  ino = cwd->ino;

  pino = search(cwd->dev, cwd, "..");

  pInode = iget(cwd->dev, pino, minode);

  for (i = 0; i < 15; i++)
  {
    i_blocks[i] = pInode->INODE.i_block[i];
  }

  maxBytes = pInode->INODE.i_size;

  rpwd(pInode, root, minode);
  for (i = 0; i < 12 && readBytes < maxBytes; i++)
  {
    if (i_blocks[i] == 0)
    {
      printf("i_block[%d] is 0\n", i);
      break;
    }
    get_block(cwd->dev, i_blocks[i], buf);
    dir = (DIR *)buf;
    if (dir->rec_len == 0)
    {
      printf("i_block[%d] dir 0 len is 0\n", i);
      break;
    }
    cp = buf;
    while (cp < buf + BLOCK_SIZE)
    {
      strncpy(filename, dir->name, dir->name_len);
      filename[dir->name_len] = 0;
      if (dir->inode == ino)
      {
        printf("/%s", filename);
      }
      readBytes += dir->rec_len;
      cp += dir->rec_len;
      dir = (DIR *)cp;
    }
  }
}

void pwd(MINODE *cwd, MINODE *root, MINODE *minode)
{
  if (cwd->dev == root->dev && cwd->ino == root->ino)
    printf("/\n");
  else
    rpwd(cwd, root, minode);

  printf("\n");
}
