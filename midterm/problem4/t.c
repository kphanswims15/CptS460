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

#include "type.h"
#include "string.c"
#include "queue.c"
#include "kbd.c"
#include "uart.c"
#include "vid.c"
#include "exceptions.c"
#include "kernel.c"
#include "timer.c"
#include "pipe.c"

PIPE *kpipe;
UART *up;

int color;

int pipe_write()
{
  struct uart *up = &uart[0];
  char line[128];
  while(1)
  {
    fuprints(up, "Enter a line for task to get : ");
    kprintf("task %d waits for line from UART0 \n", running->pid);
    ugets(up, line);
    uprints(up, "\r\n");
    if (strcmp(line, "exit") != 0)
    {
      killpipe(running);
      write_pipe(kpipe, line, strlen(line));
    }
    printf("task %d writes line = [%s] to pipe\n", running->pid, line);
    write_pipe(kpipe, line, strlen(line));
  }
}

int pipe_reader()
{
  char line[128];
  int i, n;
  while(1)
  {
    printf("task %d reading from pipe\n", running->pid);
    n = read_pipe(kpipe, line, 20);
    printf("task %d read n = %d bytes from pipe: []", running->pid, n);
    for(i = 0; i < n; i++)
      kputc(line[i]);
    printf("]\n");
  }
}

void copy_vectors(void) {
    extern u32 vectors_start;
    extern u32 vectors_end;
    u32 *vectors_src = &vectors_start;
    u32 *vectors_dst = (u32 *)0;

    while(vectors_src < &vectors_end)
       *vectors_dst++ = *vectors_src++;
}

void IRQ_handler()
{
    int vicstatus, sicstatus;
    int ustatus, kstatus;

    // read VIC status register to find out which interrupt
    vicstatus = VIC_STATUS; // VIC_STATUS=0x10140000=status reg
    sicstatus = SIC_STATUS;
    //kprintf("vicstatus=%x sicstatus=%x\n", vicstatus, sicstatus);
    if (vicstatus & 0x80000000){
       if (sicstatus & 0x08){
         kbd_handler();
       }
    }

    if (vicstatus & (1<<4))
    {
      timer_handler(0);
    }
}
int body(int pid, int ppid, int func, int priority);
int main()
{
   int i;
   char line[128];
   u8 kbdstatus, key, scode;

   color = WHITE;
   row = col = 0;

   fbuf_init();

   /* enable timer0,1, uart0,1 SIC interrupts */
   VIC_INTENABLE |= (1<<4);  // timer0,1 at bit4
   VIC_INTENABLE |= (1<<5);  // timer2,3 at bit5

   VIC_INTENABLE |= (1<<31); // SIC to VIC's IRQ31

   /* enable KBD IRQ */
   SIC_ENSET |= 1<<3;     // KBD int=3 on SIC
   SIC_PICENSET = 1<<3;  // KBD int=3 on SIC

   timer_init();
   kbd_init();
   uart_init();
   pipe_init();
   kpipe = create_pipe();
   timer_start(0);
   /* enable KBD IRQ */

   kprintf("Welcome to WANIX in Arm\n");
   init();


   //fuprints(up, "hi");
   //kfork((int) body, 1);
   kfork((int) pipe_write, 1);
   kfork((int) pipe_reader, 1);
   //kfork((int)body, 1);

   while(1){
     if (readyQueue)
        tswitch();
   }
}
