#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <ext2fs/ext2_fs.h>
#include <stdint.h>
#include <string.h>
#include "types.h"
#include "utils.h"
#include "commands.h"

int parseInput(char *line, char **myargv[])
{
    char *token = NULL, **tmp = NULL;
    int i = 0;
    size_t size = 0;

    token = strtok(line, " ");
    while (token != NULL)
    {
        tmp = (char **)realloc(*myargv, size + sizeof(char *));
        if (tmp != NULL)
        {
            *myargv = tmp;
        }
        else
        {
            printf("Realloc failed.\n");
            return -1;
        }
        size += sizeof(char *);
        (*myargv)[i] = NULL;
        (*myargv)[i] = realloc((*myargv)[i], sizeof(char) * (strlen(token) + 1));
        strcpy((*myargv)[i], token);
        token = strtok(NULL, " ");
        i++;
    }
    return i;
}

int main(int argc, char *argv[])
{
    char *dev = NULL, line[MAX];
    PROC *processes = NULL;
    PROC *running = NULL;
    MINODE *minode = NULL;
    MINODE *root = NULL;
    struct mntTable *mtable = NULL;
    int fd, numArgs, cmdIndex, count, size;
    char **myargv = NULL;
    char *lineCopy = NULL;
    char *buf = NULL;
    char *name = NULL;

    if (argc < 2)
    {
        printf("Usage: %s [disk filename]\n", argv[0]);
        exit(1);
    }

    dev = argv[1];

    init(&processes, &minode, 100, &root);
    fd = mount_root(dev, &root, minode, &running, &processes, &mtable);

    if (fd < 0)
    {
        printf("Disk %s does not exist", argv[1]);
        return 2;
    }

    print_minode(root);

    while (1)
    {
        printf("Input a command : ");

        bzero(line, MAX);
        fgets(line, MAX, stdin);

        line[strlen(line) - 1] = 0;
        if (line[0] == 0)
            exit(0);

        printf("Line: %s\n", line);

        lineCopy = (char *)malloc(sizeof(char) * (strlen(line) + 1));
        strcpy(lineCopy, line);
        myargv = NULL;
        numArgs = parseInput(lineCopy, &myargv);
        if (numArgs < 2)
        {
            myargv = (char **)realloc(myargv, 2 * sizeof(char *));
            myargv[1] = NULL;
        }
        if (numArgs < 3)
        {
            myargv = (char **)realloc(myargv, 3 * sizeof(char *));
            myargv[2] = NULL;
        }
        free(lineCopy);
        lineCopy = NULL;
        cmdIndex = findCmd(myargv[0]);

        switch(cmdIndex)
        {
        //  cmd = ls:
            case 0: ls(myargv[1], root, running, minode);
                    myargv[1] = NULL;
                    break;
        //  cmd = cd:
            case 1: chdirec(myargv[1], running, root, minode);
                    myargv[1] = NULL;
                    break;
        //  cmd = pwd:
            case 2: pwd(running->cwd, root, minode);
                    break;
            // cmd = mkdir
            case 3: makedir(myargv[1], running->cwd, root, running, minode);
                    break;
            case 4: quit(minode);
                    break;
            default: printf("Command does not exist\n");
                     break;
        }
        myargv = NULL;
    }
    return 0;
}
