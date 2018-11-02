#ifndef __LINKS_H__
#define __LINKS_H__

#include <time.h>
#include <libgen.h>
#include "types.h"
#include "utils.h"

int readlink_file(char *filename, char **buffer, MINODE *root, PROC *running, MINODE *minode);

#endif
