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

#include "defines.h"
#include "string.c"

int color;
char *tab = "0123456789ABCDEF";


#include "timer.c"
#include "vid.c"
#include "queue.c"
#include "kernel.c"
#include "uart.c"
#include "interrupts.c"
#include "kbd.c"

UART *up;

void copy_vectors(void) {
    extern u32 vectors_start;
    extern u32 vectors_end;
    u32 *vectors_src = &vectors_start;
    u32 *vectors_dst = (u32 *)0;

    while(vectors_src < &vectors_end)
       *vectors_dst++ = *vectors_src++;
}
int kprintf(char *fmt, ...);
void timer_handler();

// IRQ interrupts handler entry point
// void __attribute__((interrupt)) IRQ_handler()
// timer0 base=0x101E2000; timer1 base=0x101E2020
// timer3 base=0x101E3000; timer1 base=0x101E3020
// currentValueReg=0x04
TIMER *tp[4];

int getLowest5(int i)
{
  return i & (1 << 5)-1;
}

void printRegMode(int i)
{
  int mode = getLowest5(i);

  if(mode == SVC)
  {
    printf("SVC mode\n\r");
  }
  else if(mode == IRQ)
  {
    printf("IRQ mode\n\r");
  }
  else
  {
    printf("Wrong F***ing mode\n\r");
  }
}

int printStatus = 1;
void IRQ_handler()
{
    int vicstatus, sicstatus;
    int ustatus, kstatus;

    // read VIC SIV status registers to find out which interrupt
    vicstatus = VIC_STATUS;
    sicstatus = SIC_STATUS;
    // kprintf("vicstatus=%x sicstatus=%x\n", vicstatus, sicstatus);
    // VIC status BITs: timer0,1=4, uart0=13, uart1=14, SIC=31: KBD at 3
    /**************
    if (vicstatus & 0x0010){   // timer0,1=bit4
      if (*(tp[0]->base+TVALUE)==0) // timer 0
         timer_handler(0);
      if (*(tp[1]->base+TVALUE)==0)
         timer_handler(1);
    }
    if (vicstatus & 0x0020){   // timer2,3=bit5
       if(*(tp[2]->base+TVALUE)==0)
         timer_handler(2);
       if (*(tp[3]->base+TVALUE)==0)
         timer_handler(3);
    }
    if (vicstatus & 0x80000000){
       if (sicstatus & 0x08){
          kbd_handler();
       }
    }
    *********************/
    /**********
    if (vicstatus & (1<<4)){   // timer0,1=bit4
      if (*(tp[0]->base+TVALUE)==0) // timer 0
         timer_handler(0);
      if (*(tp[1]->base+TVALUE)==0)
         timer_handler(1);
    }
    if (vicstatus & (1<<5)){   // timer2,3=bit5
       if(*(tp[2]->base+TVALUE)==0)
         timer_handler(2);
       if (*(tp[3]->base+TVALUE)==0)
         timer_handler(3);
    }
    *****************/
    if (vicstatus & (1<<4)){   // timer0,1=bit4
         timer_handler(0);
    }
    if (vicstatus & (1<<31)){
      if (sicstatus & (1<<3)){
          kbd_handler();
       }
    }

    /*if ((*(up->base + FR) & RXFE) == 0)
      kwakeup(up->base + DR);
    if ((*(up->base + FR) & TXFF) == 0)
      kwakeup(up->base + FR);*/

    if (printStatus){
      printStatus=0;
      printf("In IRQ_handler     current mode: ");
      printRegMode(getcpsr());
      printf("\n\r");
      // prev onecwas
      printf("In IRQ_handler     previous mode: ");
      printRegMode(getspsr());
      printf("\n\r");
    }
}

int main()
{
  int i;
   char line[128], line2[128];
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
   timer_start(0);
   /* enable KBD IRQ */

   kprintf("Welcome to WANIX in Arm\n");
   init();
   uart_init();

   //kfork((int)uartDriver, 1);
   kfork((int)body, 1);
   //kfork((int)kbdTest, 1);
   while(1){
     color = CYAN;
      kprintf("Enter a line from KBD\n");
      kgets(line);
      color = CYAN;
      kprintf("line = %s\n", line);
     /*if (readyQueue)
        tswitch();*/
   }

}
