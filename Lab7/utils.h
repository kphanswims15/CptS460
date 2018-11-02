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

int get_block(int fd, int blk, char *buf);
int put_block(int fd, int blk, char *buf);
MINODE *iget(int dev, int ino, MINODE *minode);
void iput(MINODE *mip, MINODE *minode);
int ialloc(int dev, MINODE *root);
void idalloc(int dev, int ino, MINODE *root);
int balloc(int dev, MINODE *root);
void bdalloc(int dev, int bno, MINODE *root);
int search(int dev, MINODE *mip, char *name);
int tokenize(char *buf, char **names[]);
int getino(char *pathname, MINODE *root, PROC *running, MINODE *minode);
int get_super_block(int dev, struct ext2_super_block **sb);
void print_super_block(int dev);
void print_group_info(int dev, int group_num, int first_data_block);
void print_minode(MINODE *minode);
void print_inode(INODE *inode);
void init(PROC *p[], MINODE *minode[], int numMInodes, MINODE **root);
int mount_root(char *devName, MINODE **root, MINODE *minode, PROC **running, PROC *p[], struct mntTable *mtable[]);
#endif //__UTIL_H__
