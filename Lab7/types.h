#ifndef __TYPES_H__
#define __TYPES_H__

/********** type.h **********/
#include <ext2fs/ext2_fs.h>

typedef unsigned char u8;
typedef unsigned char u16;
typedef unsigned char u32;

typedef struct ext2_super_block SUPER;
typedef struct ext2_group_desc  GD;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;

SUPER *sp;
GD    *gp;
INODE *ip;
DIR   *dp;

#define FREE  0
#define READY 1

#define BLKSIZE 1024
#define NMINODE 64
#define NFD     16
#define NMOUNT  4
#define NPROC   2

typedef struct minode{
  INODE INODE;
  int dev, ino;
  int refCount;
  int dirty;
  int mounted;
  struct mntable *mptr;
}MINODE;

typedef struct oft{
  int  mode;
  int  refCount;
  MINODE *mptr;
  int  offset;
}OFT;

typedef struct proc{
  struct proc *next;
  int          pid;
  int          ppid;
  int          status;
  int          uid, gid;
  MINODE      *cwd;
  OFT         *fd[NFD];
}PROC;

typedef struct mntTable{
  int dev;
  int nblock;
  int ninodes;
  int bmap;
  int imap;
  int iblock;
  MINODE *mountDirPtr;
  char devName[64];
  char mntName[64];
} MTABLE;

#endif
