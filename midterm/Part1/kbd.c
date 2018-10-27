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

#include "keymap"
#define KCNTL 0x00
#define KSTAT 0x04
#define KDATA 0x08
#define KCLK  0x0C
#define KISTA 0x10
#define LEFTS 0x12
#define RIGHTS 0x59

typedef volatile struct kbd{
  char *base;
  char buf[128];
  int head, tail, data, room;
}KBD;

volatile KBD kbd;

int kbd_init()
{
  KBD *kp = &kbd;
  kp->base = (char *)0x10006000;
  *(kp->base + KCNTL) = 0x14;
  *(kp->base + KCLK)  = 8;
  kp->head = kp->tail = 0;
  kp->data = 0; kp->room = 128;
}

// keeps track of when the keys are down
u8 keysDown[256];

// marks that the key is down
void keyDown(u8 keyCode)
{
  keysDown[keyCode] = 1;
}

// marks that the key is up
void keyUp(u8 keyCode)
{
  keysDown[keyCode] = 0;
}

// checks if the key is down
int isKeyDown(u8 keyCode)
{
  if (keysDown[keyCode] == 1)
    return 1;
  else
    return 0;
}

void kbd_handler()
{
  u8 scode, c, keyStatus;
  KBD *kp = &kbd;
  color = YELLOW;
  scode = *(kp->base + KDATA);

  // catches the release character
  if (scode == 0xF0)
    return;

  // releases the key if it is down
  if (isKeyDown(scode) == 1)
  {
    keyUp(scode);
    return;
  }

  // key needs to be up and ready to use
  keyDown(scode);

  // check if the shifts are down
  if (isKeyDown(LEFTS) || isKeyDown(RIGHTS))
    // uppercase
    c = utab[scode];
  else
    // lower case
    c = ltab[scode];

  if (c >= 'a' && c <= 'z' || c == '\b' || c == '\r' || c >= 'A' && c <= 'Z' || c == ' ' || c >= '0' && c <= '9')
  {
    if (c == '\r')
      printf("\n");
    printf("%c", c);
     //printf("kbd interrupt: c=%x %c\n", c, c);
  }

  kp->buf[kp->head++] = c;
  kp->head %= 128;
  kp->data++;
  kwakeup(&kp->data);
}

int kgetc()
{
  char c;
  KBD *kp = &kbd;

  while(1)
  {
      lock();                 // disable IRQ interrupts
      if (kp->data == 0)      // check data with IRQ disable
      {
        unlock();             // enable IRQ interrupts
        ksleep(&kp->data);     // sleep for data
      }
      else
      {
        break;
      }
  }

  c = kp->buf[kp->tail++];    // get a c and uptdate tail index
  kp->tail %= 128;
  kp->data--;                 // update with interrupts OFF
  unlock();                   // enable IRQ interrupts

  return c;
}

int kgets(char s[ ])
{
  char c;
  while( (c = kgetc()) != '\r'){
    *s++ = c;
  }
  *s = 0;
  return strlen(s);
}

/*int kbdTest()
{
  char line[128];

   color = CYAN;
   kprintf("Enter a line from KBD\n");
   kgets(line);
   color = CYAN;
   kprintf("line = %s\n", line);
}*/
