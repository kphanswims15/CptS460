#include "directories.h"

int chdirec(char *pathname, PROC *running, MINODE *root, MINODE *minode)
{
    MINODE *mip;
    int ino;

    if (pathname == NULL)
    {
        return 0;
    }

    if (pathname[0] == '\0')
    {
        // goes to root if there is no pathname
        running->cwd = iget(root->dev, 2, minode);
        return 1;
    }
    else
    {
        // cd to the pathname
        ino = getino(pathname, root, running, minode);
        mip = iget(root->dev, ino, minode);

        // checking if it is a directory
        if ((mip->INODE.i_mode & 0xF000) == 0x4000)
        {
            iput(running->cwd, minode);
            running->cwd = mip;
            return 1;
        }
        else
        {
            printf("%s is not a directory\n", pathname);
            return -1;
        }
    }
}

void rpwd(MINODE *cwd, MINODE *root, MINODE *minode)
{
    int i, ino, pino, maxBytes = 0, readBytes = 0, i_blocks[15];
    char *cp = NULL, buf[BLOCK_SIZE], filename[MAX];
    MINODE *pInode = NULL;
    DIR *dir = NULL;

    if (cwd->dev == root->dev && cwd->ino == root->ino) return;

    ino = cwd->ino;

    pino = search(cwd->dev, cwd, "..");

    pInode = iget(cwd->dev, pino, minode);

    for (i = 0; i < 15; i++)
    {
        i_blocks[i] = pInode->INODE.i_block[i];
    }

    maxBytes = pInode->INODE.i_size;

    rpwd(pInode, root, minode);
    for (i = 0; i < 12 && readBytes < maxBytes; i++)
    {
        if (i_blocks[i] == 0)
        {
            printf("i_block[%d] is 0\n", i);
            break;
        }
        get_block(cwd->dev, i_blocks[i], buf);
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
            if (dir->inode == ino)
            {
                printf("/%s", filename);
            }
            readBytes += dir->rec_len;
            cp += dir->rec_len;
            dir = (DIR *)cp;
        }
    }
}

void pwd(MINODE *cwd, MINODE *root, MINODE *minode)
{
    if (cwd->dev == root->dev && cwd->ino == root->ino)
        printf("/\n");
    else
        rpwd(cwd, root, minode);

    printf("\n");
}

void makedir(char *pathname, MINODE *cwd, MINODE *root, PROC *running, MINODE *minode)
{
    char *parentPath = NULL, *childName = NULL, *pathCpy = NULL, buf[BLOCK_SIZE];
    int pIno, cIno, cBlk, i;
    MINODE *pMip = NULL, *cMip = NULL;
    INODE *ip = NULL;

    if (pathname == NULL)
    {
        printf("Must provide a directory to create for mkdir");
        return;
    }

    pathCpy = (char *)malloc(sizeof(char) * strlen(pathname));
    strcpy(pathCpy, pathname);

    // Use dirname and basename to divide pathname into parent and child
    parentPath = dirname(pathCpy);
    childName = basename(pathname);

    // Confirm parent exists and is a DIR
    pIno = getino(parentPath, root, running, minode);
    pMip = iget(root->dev, pIno, minode);
    if ((pMip->INODE.i_mode & 0xF000) != 0x4000)
    {
        printf("%s is not a directory. Cannot mkdir within a file\n", parentPath);
        return;
    }

    // Make sure that the child does not exist in the parent DIR
    cIno = search(root->dev, pMip, childName);
    if (cIno)
    {
        printf("%s already exists!\n", childName);
        return;
    }

    // Create DIR

        // Allocate an INODE and a disk block
        cIno = ialloc(root->dev, root);
        cBlk = balloc(root->dev, root);

        // Load INODE into minode
        cMip = iget(root->dev, cIno, minode);

        // Initialize cMip->INODE as a DIR
        ip = &(cMip->INODE);
        ip->i_mode = 0x41ED;
        ip->i_uid = running->uid;
        ip->i_gid = running->gid;
        ip->i_size = BLOCK_SIZE;
        ip->i_links_count = 2;
        ip->i_atime = ip->i_ctime = ip->i_mtime = time(0L);
        ip->i_blocks = 2;

        cMip->INODE.i_block[0] = cBlk;
        for (i = 1; i < 15; i++)
        {
            cMip->INODE.i_block[i] = 0;
        }
        cMip->dirty = 1;
        iput(cMip, minode);

        // Create . and .. dir entries
        bzero(buf, BLOCK_SIZE);
        get_block(root->dev, cBlk, buf);
        DIR *dp = (DIR *)buf;
        // make . entry
        dp->inode = cIno;
        dp->rec_len = 12;
        dp->name_len = 1;
        dp->name[0] = '.';
        dp->name[1] = '\0';

        // make .. entry
        dp = (DIR *)((char *)dp + 12);
        dp->inode = pIno;
        dp->rec_len = BLOCK_SIZE - 12;
        dp->name_len = 2;
        dp->name[0] = dp->name[1] = '.';
        dp->name[2] = '\0';
        put_block(root->dev, cBlk, buf);

        // Add child to parent INODE
        enter_child(pMip, cIno, childName, root);

    // Increment parent INODE links_count by 1 and mark parent as dirty
    pMip->INODE.i_links_count++;
    pMip->dirty = 1;
    iput(pMip, minode);

    get_block(pMip->dev, 2, buf);
    ((GD *)buf)->bg_used_dirs_count++;
    put_block(pMip->dev, 2, buf);
}
