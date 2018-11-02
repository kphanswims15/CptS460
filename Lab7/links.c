#include "links.h"

int readlink_file(char *filename, char **buffer, MINODE *root, PROC *running, MINODE *minode)
{
    int ino;
    MINODE *mip;

    // Get file's INODE
    ino = getino(filename, root, running, minode);
    if (ino == 0)
    {
        printf("%s does not exist\n", filename);
        return -1;
    }

    mip = iget(root->dev, ino, minode);

    // Verify that it is a link
    if ((mip->INODE.i_mode & 0xF000) != 0xA000)
    {
        printf("%s is not a link\n", filename);
        return -1;
    }

    // Copy target filename into buffer
    *buffer = (char *)malloc(sizeof(char *) * (mip->INODE.i_size + 1));
    memcpy(*buffer, mip->INODE.i_block, mip->INODE.i_size);
    (*buffer)[mip->INODE.i_size] = '\0';

    // Return file size
    return mip->INODE.i_size;
}
