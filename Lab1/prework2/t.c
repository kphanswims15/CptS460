/*******************************************************
*                      t.c file                        *
*******************************************************/
typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;

#define TRK 18
#define CYL 36
#define BLK 1024

#include "ext2.h"
typedef struct ext2_group_desc  GD;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;
GD    *gp;
INODE *ip;
DIR   *dp;

char buf1[BLK], buf2[BLK];
int color = 0x0A;
u8 ino;

int prints(char *s);
int gets(char *s);

main()
{
  u16    i, iblk;
  char   c, temp[64];
  char *cp;

  prints("read block# 2 (GD)\n\r");
  getblk(2, buf1);

// 1. WRITE YOUR CODE to get iblk = bg_inode_table block number
  gp = (GD *)buf1;
  iblk = gp->bg_inode_table;
  prints("inode_block="); putc(iblk + '0'); prints("\n\r");

// 2. WRITE YOUR CODE to get root inode
  getblk(iblk, buf1);
  ip = (INODE *)buf1 + 1;
  prints("read inodes begin block to get root inode\n\r");

// 3. WRITE YOUR CODE to step through the data block of root inode
   /*for(i = 0; i < 12; i++)
   {
     if(i < 10)
     {
       prints("Block, i = ");
       putc(i + '0');
       prints("\n\r");
     }
     else if(i == 10)
     {
       prints("Block, i = 10\n\r");
     }
     else
     {

     }
   }
   prints("read data block of root DIR\n\r");

// 4. print file names in the root directory /
   while(cp < buf2 + 1024)
   {
     prints(dp->name);
     getc();
     cp += dp->rec_len;
     dp = (DIR *)cp;
   }*/

}

int prints(char *s)
{
  while(*s != '\0')
  {
    putc(*s);
    s++;
  }
  return 1;
}

int gets(char *s)
{
  while((*s = getc()) != '\r')
  {
    putc(*s++);
  }
  *s = 0;
  return 1;
}

int getblk(u16 blk, char *buf)
{
  // readfd( (2*blk)/CYL, ( (2*blk)%CYL)/TRK, ((2*blk)%CYL)%TRK, buf);
  readfd( blk/18, ((blk)%18)/9, (((blk)%18)%9)<<1, buf);
}
