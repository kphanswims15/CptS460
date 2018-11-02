#ifndef __DIRECTORIES_H__
#define __DIRECTORIES_H__

#include <libgen.h>
#include <time.h>
#include "types.h"
#include "utils.h"

#ifndef MAX
#define MAX 256
#endif

int chdirec(char *pathname, PROC *running, MINODE *root, MINODE *minode);
void rpwd(MINODE *cwd, MINODE *root, MINODE *minode);
void pwd(MINODE *cwd, MINODE *root, MINODE *minode);
void makedir(char *pathname, MINODE *cwd, MINODE *root, PROC *running, MINODE *minode);

#endif //__DIRECTORIES_H__
