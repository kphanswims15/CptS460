/*******************************************************
*                  @t.c file                          *
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


u16 NSEC = 2;

u16 getblk(u16 blk, char *buf)
{
    //readfd( (2*blk)/CYL, ( (2*blk)%CYL)/TRK, ((2*blk)%CYL)%TRK, buf);

    readfd( blk/18, ((blk)%18)/9, ( ((blk)%18)%9)<<1, buf);
}

INODE *search(INODE *ip, char *name, u16 iblk)
{
  u8 i;
  DIR   *dp;
  char *tmp, buf[BLK];

  // looks through the blocks
  for (i = 0; i < 12; i++)
  {
    // breaks if the file doesn't exist
    if (ip->i_block[i] == 0) break;

    getblk((u16)ip->i_block[i], buf);

    dp = (DIR *)buf;
    tmp = buf;

    while (tmp < buf + BLK)
    {
      // check if the exists
      if (strncmp(name, dp->name, dp->name_len) == 0)
      {
        // return the inode if it does
        i = dp->inode - 1;
        getblk((i / 8) + iblk, buf);
        return (INODE *)buf + (i % 8);
      }
        // checks the next file
        tmp += dp->rec_len;
        dp = (DIR*)tmp;
      }
    }

  return 0;
}

main()
{
  GD    *gp;
  INODE *ip;
  DIR   *dp;
  u16  i, iblk;
  u32 *iblk2;
  char buf1[BLK], buf2[BLK];

  getblk((u16)2, buf1);
  gp = (GD *)buf1;
  iblk = (u16)gp->bg_inode_table;
  getblk(iblk, buf1);
  ip = (INODE *)buf1 + 1;

  prints("Boot: mtx\n\r");

  // 1. Write YOUR C code to getthe INODE of /boot/mtx
  // checks if boot exists in root
  ip = search(ip, "boot", iblk);
  // it is an error if not found
  if (ip == 0) error();

  // check if mtx exists in boot
  ip = search(ip, "mtx", iblk);
  // if not found it is an error
  if (ip == 0) error();

  // get mtx's info
  i = ip->i_block[12];
  getblk(i, buf2);
  iblk2 = buf2;

  // 2. load the blocks into memory at 0x1000.
  setes(0x1000); // set segment 0x1000 up...

  // 3. load 12 DIRECT blocks of INODE into memory
  for (i=0; i<12; i++){
      getblk((u16)ip->i_block[i], 0);
      putc('*');
      inces();
   }

   // 4. load INDIRECT blocks, if any, into memory
   while (*iblk2 != 0)
   {
       getblk(*iblk2++, 0);
       inces();
  }

  prints("go?"); getc();
}
