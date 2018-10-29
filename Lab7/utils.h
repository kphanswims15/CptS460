#ifndef __UTIL_H__
#define __UTIL_H__

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "types.h"

#define BLOCK_SIZE 1024

void init(PROC *p[], MINODE *minode[], int numMInodes, MINODE **root);
int get_block(int fd, int blk, char *buf);
MINODE *iget(int dev, int ino, MINODE *minode);
int mount_root(char *devName, MINODE **root, MINODE *minode, PROC **running, PROC *p[], struct mntTable **mtables);
