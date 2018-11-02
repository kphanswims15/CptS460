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
int put_block(int fd, int blk, char *buf);
MINODE *iget(int dev, int ino, MINODE *minode);
void iput(MINODE *mip, MINODE *minode);
int search(int dev, MINODE *mip, char *name);
int tokenize(char *buf, char **names[]);
int getino(char *pathname, MINODE *root, PROC *running, MINODE *minode);
int mount_root(char *devName, MINODE **root, MINODE *minode, PROC **running, PROC *p[], struct mntTable **mtables);
void print_minode(MINODE *minode);
void print_inode(INODE *inode);
int parseInput(char *line, char **myargv[]);
