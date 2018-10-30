int exec(char *cmdline) // cmdline=VA in Uspace
{
  // cmdline = cmd arg1 .... argn
  // load /bin/cmd to Umode iamge; passing cmdline to new image
  // let caller return to VA(0) in Umode, with cmdline pointed by R0

  int i, upa, usp;
  char *cp, kline[128], file[32], filename[32];
  PROC *p = running;

  kstrcpy(kline, cmdline); // fetch cmdline into kernel space
  printf("EXEC: proc %d cmdline = %x\n", running->pid, cmdline);

  // get first token of kline as filename
  cp = kline;
  i = 0;

  while(*cp != ' ')
  {
    filename[i] = *cp;
    i++;
    cp++;
  }

  filename[i] = 0;
  file[0] = 0;
  if (filename[0] != '/') // if filename is relative
    kstrcpy(file, filename);
  kstrcat(file, filename);

  upa = p->pgdir[2048] & 0xFFFF0000; // PA of Umode image
  printf("load file %s to %x\n", file, upa);

  // load filename to Umode Uimage
  load(file, p);
  printf("after loading ");

  // copy kline to high end of Ustatck in Umode image
  usp = upa + 0x100000 - 128;
  printf("usp = %x ", usp);
  kstrcpy((char *)usp, kline);

  p->usp = (int *)VA(0x100000 = 128);
  printf("p->usp = %x ", p->usp);

  // set up syscall kstack frame to return to new image at VA=0
  //   usp
  // --|-----goUmode--------------------------------
  //  r0 r1 r2 r3 r4 r5 r6 r7 r8 r9 r10 r11 r12 ULR|
  //-------------------------------------------------
  //  14 13 12 11 10 9  8  7  6  5  4   3    2   1
  for (i = 2; i < 14; i++)
    p->kstack[SSIZE - i] = 0;
  p->kstack[SSIZE - 14] = (int)VA(p->usp);
  p->kstack[SSIZE - 1] = (int)VA(0);

  printf("kexec exit\n");
  return (int)p->usp; // will replace saved r0 in kstack
}
