/********************************************************************
Copyright 2010-2017 K.C. Wang, <kwang@eecs.wsu.edu>
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/

/********* SDC Driver sdc.c **********
This SDC driver read/write 1KB blocks. It uses
semaphores to synchronize driver and interrupt handler
Consult Chapter 8 of text for reason WHY.
***************************************/
#define enterCR  ps=int_off()
#define exitCR   int_on(ps)

extern PROC *readyQueue;
extern int how;  // BUSYWAIT or NOBUSYWAIT

typedef struct semaphore{
  int value;
  struct proc *queue;
}SEMAPHORE;

void P(struct semaphore *s)
{
  int ps;

  ps = int_off();
    s->value--;
    if (s->value < 0){
       // printf("P: block %d\n", running->pid);
       running->status = BLOCK;
       enqueue(&s->queue, running);
       tswitch();
    }
  int_on(ps);
}

void V(struct semaphore *s)
{
  PROC *p; int ps;
  ps=int_off();
    s->value++;
    if (s->value <= 0){
        p = (PROC *)dequeue(&s->queue);
        p->status = READY;
        enqueue(&readyQueue, p);
    }
  int_on(ps);
}

#include "sdc.h"
u32 base;  //SDC base addres in I/O map

int delay() // delay loop after issuing a SDC command: optional
{  int i; for (i=0; i<1000; i++); }

int do_command(int cmd, int arg, int resp)
{
  *(u32 *)(base + ARGUMENT) = (u32)arg;
  *(u32 *)(base + COMMAND)  = 0x400 | (resp<<6) | cmd;
  //delay();
}

// shared variables between SDC driver and interrupt handler
char *rxbuf, *txbuf;
int  rxcount, txcount, rxdone, txdone;
struct semaphore rxsem, txsem; //synchronizing semaphores

int sdc_handler()
{
  u32 status, status_err;
  int i;
  u32 *up;
  int oldcolor;

  //if (rxcount==BLKSIZE)
  // printf("SDC interrupt ");

  // read status register to find out TXempty or RxAvail
  status = *(u32 *)(base + STATUS);
  oldcolor = color;

  if (status & (1<<17)){ // RxFull: read 16 u32 at a time;
     color = CYAN;
     //printf("RX interrupt: ");
     up = (u32 *)rxbuf;
     status_err = status & (SDI_STA_DCRCFAIL|SDI_STA_DTIMEOUT|SDI_STA_RXOVERR);
     if (!status_err && rxcount) {
       //printf("R%d ", rxcount);
        for (i = 0; i < 16; i++)
           *(up + i) = *(u32 *)(base + FIFO);
        up += 16;
        rxcount -= 64;
        rxbuf += 64;
	status = *(u32 *)(base + STATUS); // read status to clear Rx interrupt
     }
     color = oldcolor;
     if (rxcount == 0){
        do_command(12, 0, MMC_RSP_R1); // stop transmission

	if (how == BUSYWAIT)
	  rxdone = 1;
	else
	  V(&rxsem);
	//printf("SDC interrupt handler done ");
     }
  }
  else if (status & (1<<18)){ // TXempty: write 16 u32 at a time
    color = YELLOW;
    printf("TX interrupt: ");
    up = (u32 *)txbuf;
    status_err = status & (SDI_STA_DCRCFAIL | SDI_STA_DTIMEOUT);

    if (!status_err && txcount) {
      // printf("W%d ", txcount);
       for (i = 0; i < 16; i++)
           *(u32 *)(base + FIFO) = *(up + i);
       up += 16;
       txcount -= 64;
       txbuf += 64;            // advance txbuf for next write
       status = *(u32 *)(base + STATUS); // read status to clear Tx interrupt
    }
    color = oldcolor;
    if (txcount == 0){
       do_command(12, 0, MMC_RSP_R1); // stop transmission

       if (how == BUSYWAIT)
          txdone = 1;
       else
          V(&txsem);

       printf("SDC interrupt handler done ");
    }
  }
  //printf("write to clear register\n");
  *(u32 *)(base + STATUS_CLEAR) = 0xFFFFFFFF;
  // printf("SDC interrupt handler done\n");
}

int verifyExt2()
{
  // verify SDC is an EXT2 FS: set globals bmap, imap and iblk
  SUPER *sp;
  GD    *gp;

  printf("read SUPER block: ");
  getblk(1, buf);
  sp = (SUPER *)&buf[0];
  printf("magic=%x nblock=%d ninodes=%d\n", sp->s_magic, sp->s_blocks_count,
        sp->s_inodes_count);

  printf("read GROUP DESCRIPTOR 0: ");
  getblk(2, buf);
  gp = (GD *)buf;
  bmap = gp->bg_block_bitmap;
  imap = gp->bg_inode_bitmap;
  iblk = gp->bg_inode_table;
  printf("bmap=%d imap=%d iblk=%d\n", bmap, imap, iblk);
  return 1;
}

int sdc_init()
{
  u32 RCA = (u32)0x45670000; // QEMU's hard-coded RCA
  base    = (u32)0x10005000; // PL180 base address
  printf("sdc_init : ");
  *(u32 *)(base + POWER) = (u32)0xBF; // power on
  *(u32 *)(base + CLOCK) = (u32)0xC6; // default CLK

  // send init command sequence
  do_command(0,  0,   MMC_RSP_NONE);// idle state
  do_command(55, 0,   MMC_RSP_R1);  // ready state
  do_command(41, 1,   MMC_RSP_R3);  // argument must not be zero
  do_command(2,  0,   MMC_RSP_R2);  // ask card CID
  do_command(3,  RCA, MMC_RSP_R1);  // assign RCA
  do_command(7,  RCA, MMC_RSP_R1);  // transfer state: must use RCA
  do_command(16, 512, MMC_RSP_R1);  // set data block length

  // set interrupt MASK0 registers bits = RxFULL(17)|TxEmpty(18)
  *(u32 *)(base + MASK0) = (1<<17)|(1<<18);

  rxsem.value = 0;
  rxsem.queue = 0;
  txsem.value = 0;
  txsem.queue = 0;

  how = BUSYWAIT;
  if (verifyExt2())
     how = NOBUSYWAIT;
  printf("SDC init done\n");
}

int getblk(int blk, char *buf)
{
  u32 cmd, arg;

  //printf("getblock %d ", blk);
  rxbuf = buf; rxcount = BLKSIZE;
  rxsem.value = 0;
  rxsem.queue = 0;
  rxdone = 0;

  *(u32 *)(base + DATATIMER) = 0xFFFF0000;
  // write data_len to datalength reg
  *(u32 *)(base + DATALENGTH) = BLKSIZE;

  cmd = 18;            // CMD17 = READ single sector; 18=read block
  arg = ((blk*2)*512); // absolute byte offset in disk
  do_command(cmd, arg, MMC_RSP_R1);

  //printf("dataControl=%x\n", 0x93);
  // 0x93=|9|0011|=|9|DMA=0,0=BLOCK,1=Host<-Card,1=Enable
  *(u32 *)(base + DATACTRL) = 0x93;

  if (how == BUSYWAIT)
    while(rxdone==0);
  else
    P(&rxsem);

  //printf("getblock return\n");
}

int putblk(int blk, char *buf)
{
  u32 cmd, arg;

  printf("putblock %d %x ", blk, buf);
  txbuf = buf; txcount = BLKSIZE;
  txsem.value = 0;
  txsem.queue = 0;
  txdone = 0;

  *(u32 *)(base + DATATIMER) = 0xFFFF0000;
  *(u32 *)(base + DATALENGTH) = BLKSIZE;

  cmd = 25;                  // CMD24 = Write single sector; 25=read block
  arg = (u32)((blk*2)*512);  // absolute byte offset in diks
  do_command(cmd, arg, MMC_RSP_R1);

  //printf("dataControl=%x\n", 0x91);
  // write 0x91=|9|0001|=|9|DMA=0,BLOCK=0,0=Host->Card, Enable
  *(u32 *)(base + DATACTRL) = 0x91; // Host->card

  if (how == BUSYWAIT)
    while(txdone == 0);
  else
    P(&txsem);
  printf("putblock return\n");
}
