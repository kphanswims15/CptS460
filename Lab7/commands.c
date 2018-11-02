
#include "commands.h"

/* CMD: Array containing strings of the available commands, null terminated */
char *cmd[] = {"ls", "cd", "pwd", "mkdir", "quit", 0};

int findCmd(char *command)
{
    int i = 0;
    command = strtok(command, "\n");

    if (command == NULL)
    {
        return -2;
    }

    while (cmd[i])
    {
        if (strcmp(command, cmd[i]) == 0)
        {
            return i;
        }
        i++;
    }
    return -1;
}

/* FLAG MASK: String containing the sequence of permissions flags */
char flagMask[] = "rwxrwxrwx";
void ls_file(int dev, int ino, char *name, MINODE *root, PROC *running,
             MINODE *minode)
{
    char mode[11], *user, *group, *date, *buf;
    struct passwd *passwdFile;
    struct group *groupFile;
    int type, flags, i, hardLinks, fileSize;
    time_t mtime;

    mode[10] = '\0';

    MINODE *node = iget(dev, ino, minode);

    type = node->INODE.i_mode & 0xF000;

    if (type == 0x8000)
    {
        mode[0] = '-';
    }
    else if(type == 0x4000)
    {
        mode[0] = 'd';
    }
    else if (type == 0xA000)
    {
        mode[0] = 'l';
    }

    flags = node->INODE.i_mode;
    for (i = 9; i > 0; i--)
    {
        if (flags & 00001)
        {
            mode[i] = flagMask[i - 1];
        }
        else
        {
            mode[i] = '-';
        }
        flags >>= 1;
    }

    hardLinks = node->INODE.i_links_count;

    errno = 0;

    // gets information from the linux file like password and username
    // gets the effective user
    passwdFile = getpwuid(node->INODE.i_uid);

    if (passwdFile == NULL)
    {
        if (errno == 0)
        {
            user = "NONE";
        }
        else
        {
            printf("Passwd error!: %s", strerror(errno));
        }
    }
    else
    {
        user = passwdFile->pw_name;
    }

    groupFile = getgrgid(node->INODE.i_gid);

    group = groupFile->gr_name;

    fileSize = node->INODE.i_size;

    mtime = node->INODE.i_mtime;
    date = ctime(&mtime);

    date = strtok(date, "\n");

    printf("%10s %d %s %s %5d %s %s", mode, hardLinks, user, group, fileSize,
                                     date, name);

    if (type == 0xA000)
    {
        // This is a symlink, add the correct output
        readlink_file(name, &buf, root, running, minode);
        printf(" -> %s", buf);
    }

    printf("\n");

    iput(node, minode);
}

void ls(char *name, MINODE *root, PROC *running, MINODE *minode)
{
    char *cp = NULL, buf[BLOCK_SIZE], filename[MAX];
    MINODE *node = NULL;
    DIR *dir = NULL;
    int ino, dev = running->cwd->dev, i, readBytes = 0, maxBytes = 0, i_blocks[15];

    if (name == NULL)
    {
        name = ".";
    }

    ino = getino(name, root, running, minode);

    node = iget(dev, ino, minode);

    if (node == NULL)
    {
        printf("%s does not exist", name);
    }

    for (i = 0; i < 15; i++)
    {
        i_blocks[i] = node->INODE.i_block[i];
    }

    maxBytes = node->INODE.i_size;
    for (i = 0; i < 12 && readBytes < maxBytes; i++)
    {
        printf("Reading i_block[%d] %d...\n", i, i_blocks[i]);
        if (i_blocks[i] == 0)
        {
            printf("i_block[%d] is 0\n", i);
            break;
        }
        get_block(dev, i_blocks[i], buf);
        dir = (DIR *)buf;
        if (dir->rec_len == 0)
        {
            printf("i_block[%d] dir 0 len is 0\n", i);
            break;
        }
        cp = buf;
        while (cp < buf + BLOCK_SIZE)
        {
            strncpy(filename, dir->name, dir->name_len);
            filename[dir->name_len] = 0;
            ls_file(dev, dir->inode, filename, root, running, minode);
            readBytes += dir->rec_len;
            cp += dir->rec_len;
            dir = (DIR *)cp;
        }
    }
    iput(node, minode);
}

void quit(MINODE *minode)
{
    MINODE *mip = minode;
    while (mip->refCount != 0)
    {
        printf("Putting ino #%d\n", mip->ino);
        mip->refCount = 1;
        iput(mip, minode);
        mip++;
    }
    exit(0);
}
