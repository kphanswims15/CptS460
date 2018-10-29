#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <ext2fs/ext2_fs.h>
#include <stdint.h>
#include <string.h>
#include "types.h"
#include "utils.h"
#include "commands.h"

#define MAX 256

int main(int argc, char *argv[])
{
  char *dev = NULL, line[MAX];
  PROC *processes = NULL, *running = NULL;
  MINODE *minode = NULL, *root = NULL;
  struct mntTable *mtable = NULL;
  int fd, numArgs, cmdIndex, count, size;
  char **myargv = NULL;
  char *lineCopy = NULL, *buf = NULL, *name = NULL;

  if (argc < 2)
  {
    printf("Usage: %s [disk filename]\n", argv[0]);
    exit(1);
  }

  dev = argv[1];

  init(&processes, &minode, 100, &root);
}
