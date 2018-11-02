#ifndef __COMMANDS_H__
#define __COMMANDS_H__

#include <errno.h>
#include <grp.h>
#include <libgen.h>
#include <pwd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include "directories.h"
#include "links.h"
#include "types.h"
#include "utils.h"

#ifndef MAX
#define MAX 256
#endif

extern char *cmd[];

int findCmd(char *command);
void ls_file(int dev, int ino, char *name, MINODE *root, PROC *running, MINODE *minode);
void ls(char *name, MINODE *root, PROC *running, MINODE *minode);
void quit(MINODE *minode);

#endif //__COMMANDS_H__
