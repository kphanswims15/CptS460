#include "children.h"
void enter_child(MINODE *pip, int ino, char *child, MINODE *root)
{
    int i, ideal_length, need_length, remain, blk;
    char buf[BLOCK_SIZE];
    DIR *dp = NULL, newChild;
    char *cp = NULL;

    need_length = 4 * ((8 + strlen(child)) / 4);

    for (i = 0; i < 12; i++)
    {
        // goes through all the blocks looking for a valid block
        if (pip->INODE.i_block[i] == 0) break;

        get_block(pip->dev, pip->INODE.i_block[i], buf);

        dp = (DIR *)buf;
        cp = buf;


        while (cp + dp->rec_len < buf + BLOCK_SIZE)
        {
            cp += dp->rec_len;
            dp = (DIR *)cp;
        }

        ideal_length = 4 * ((8 + dp->name_len + 3) / 4);
        remain = dp->rec_len - ideal_length;

        if (remain >= need_length)
        {
            // Add child as new entry in this block
            break;
        }
    }

    blk = pip->INODE.i_block[i];

    // If there is not enough space in existing blocks, add a new one.
    if (blk == 0)
    {
        blk = pip->INODE.i_block[i] = balloc(pip->dev, root);
        pip->INODE.i_size += BLOCK_SIZE;
        pip->dirty = 1;
    }

    // Add child INODE as an entry here
    dp->rec_len = ideal_length;
    cp += ideal_length;

    newChild.rec_len = remain;
    newChild.inode = ino;
    newChild.name_len = strlen(child);
    strcpy(newChild.name, child);

    memcpy(cp, &newChild, sizeof(newChild));

    // Write back to disk
    put_block(pip->dev, blk, buf);
}
