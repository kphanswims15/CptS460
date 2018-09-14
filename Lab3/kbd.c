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
#define LSHFT 18
#define RSHFT 89

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

u8 keyDownMap[256];
void keyDown(u8 keyCode)
{
    keyDownMap[keyCode] = '1';
}

void keyUp(u8 keyCode)
{
  keyDownMap[keyCode] = '\0';
}

int isKeyDown(u8 keyCode)
{
    if (keyDownMap[keyCode] == '1')
    {
      return 1;
    }
    else
    {
      return 0;
    }
}

void kbd_handler()
{
  u8 scode, c;
  KBD *kp = &kbd;
  color = YELLOW;
  scode = *(kp->base + KDATA);
  if (scode & 0x80)
    return;
  //printf("%d\n\r", scode);

  if(isKeyDown(scode) == 1)
  {
    keyUp(scode);
    return;
  }

  // key must be UP and READY to USE
  keyDown(scode);

  if (isKeyDown(RSHFT) || isKeyDown(LSHFT)) {
    // UPPER
    c = toUpper(scode);
  }
  else {
    // LOWER
    c = toLower(scode);
  }

  printf("%c", c);

  kp->buf[kp->head++] = c;
  kp->head %= 128;
  kp->data++; kp->room--;
}

int kgetc()
{
  char c;
  KBD *kp = &kbd;

  unlock();
  while(kp->data == 0);

  lock();
  c = kp->buf[kp->tail++];
  kp->tail %= 128;
  kp->data--; kp->room++;
  unlock();
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
