#include "utils.h"

int get_block(int fd, int blk, char *buf)
{
    lseek(fd, (long)blk * BLOCK_SIZE, SEEK_SET);
    return read(fd, buf, BLOCK_SIZE);
}

int put_block(int fd, int blk, char *buf)
{
    lseek(fd, (long)blk * BLOCK_SIZE, SEEK_SET);
    return write(fd, buf, BLOCK_SIZE);
}

MINODE *iget(int dev, int ino, MINODE *minode)
{
    char buf[BLOCK_SIZE];
    int found = 0, block, disp;
    INODE *ip;
    MINODE *mip = minode;
    GD *gp;

    // Search for an item pointed to
    // search minode[] array for an item pointed by mip with the same
    //  (dev, ino)
    while (mip->dev < 0 || mip->refCount > 0)
    {
        if (mip->dev == dev && mip->ino == ino)
        {
            found = 1;
            break;
        }
        mip++;
    }

    if (!found)
    {
        // Inode not loaded into memory
        // searches minode array for mip with refCount = 0
        mip = minode;
        while (mip->dev < 0 || mip->refCount > 0)
        {
            mip++;
        }

        // mark it in use
        mip->refCount = 1;

        // Assign the values of dev and ino
        mip->dev = dev;
        mip->ino = ino;

        // Initialize other fields: dirty=0, mounted=0, mountPtr=0
        mip->dirty = 0;
        mip->mounted = 0;
        mip->mptr = 0;

        // Assign to blk and disp
        get_block(mip->dev, 2, buf);
        gp = (GD *)buf;

        // block that contains the inode
        block = (mip->ino - 1)/ 8 + gp->bg_inode_table;
        // which inode in the block
        disp = (mip->ino - 1) % 8;

        // Load the block into buf
        get_block(dev, block, buf);

        // Point ip to INODE in buf
        ip = ((INODE *)buf) + disp;

        // copy INODE into minode.INODE
        mip->INODE = *ip;
    }
    else
    {
        mip->refCount++;
        return mip;
    }
    return mip;
}

void iput(MINODE *mip, MINODE *minode)
{
    int block = 0, disp = 0;
    char buf[BLOCK_SIZE];
    INODE *ip;
    GD *gp;

    mip->refCount--;

    if (mip->refCount <= 0 && mip->dirty)
    {
        // INODE needs to be written back to disk

        printf("iput: dev=%d ino=%d size=%d\n", mip->dev, mip->ino, mip->INODE.i_size);

        //Calculate block and disp
        get_block(mip->dev, 2, buf);
        gp = (GD *)buf;

        // which INODE in the block
        block = (mip->ino - 1)/ 8 + gp->bg_inode_table;
        // offset of the INODE in the buf
        disp = (mip->ino - 1) % 8;

        get_block(mip->dev, block, buf);

        if (mip->INODE.i_links_count != 0)
        {
            ip = (INODE *)buf + disp;
            *ip = mip->INODE;
        }

        put_block(mip->dev, block, buf);
    }
}

int test_bit(char *buf, int bit)
{
    return buf[bit / 8] & (1 << (bit % 8));
}

void set_bit(char *buf, int bit)
{
    buf[bit / 8] |= (1 << (bit % 8));
}

void clear_bit(char *buf, int bit)
{
    buf[bit / 8] &= ~(1 << (bit % 8));
}

void decFreeInodes(int dev)
{
    char buf[BLOCK_SIZE];
    SUPER *sp;
    GD *gp;
    get_block(dev, 1, buf);
    sp = (SUPER *)buf;
    sp->s_free_inodes_count--;
    put_block(dev, 1, buf);
    get_block(dev, 2, buf);
    gp = (GD *)buf;
    gp->bg_free_inodes_count--;
    put_block(dev, 2, buf);
}

void incFreeInodes(int dev)
{
    char buf[BLOCK_SIZE];
    SUPER *sp;
    GD *gp;
    get_block(dev, 1, buf);
    sp = (SUPER *)buf;
    sp->s_free_inodes_count++;
    put_block(dev, 1, buf);
    get_block(dev, 2, buf);
    gp = (GD *)buf;
    gp->bg_free_inodes_count++;
    put_block(dev, 2, buf);
}

int ialloc(int dev, MINODE *root)
{
    int i, imap;
    char buf[BLOCK_SIZE];
    MTABLE *mp = root->mptr;

    imap = mp->imap;

    get_block(dev, imap, buf);
    for (i = 0; i < mp->ninodes; i++)
    {
        if (test_bit(buf, i) == 0)
        {
            set_bit(buf, i);
            put_block(dev, imap, buf);
            // Update free inode count in SUPER and GD
            decFreeInodes(dev);
            return (i + 1);
        }
    }

    return 0;
}

void idalloc(int dev, int ino, MINODE *root)
{
    int imap;
    char buf[BLOCK_SIZE];
    MTABLE *mp = root->mptr;

    imap = mp->imap;

    if (ino > mp->ninodes)
    {
        printf("INODE number %d out of range\n", ino);
        return;
    }

    // Get INODE bitmap block
    get_block(dev, imap, buf);
    clear_bit(buf, ino-1);

    // Write buffer back to block
    put_block(dev, imap, buf);
    // Update free inode count in SUPER and GD
    incFreeInodes(dev);
}

void decFreeBlocks(int dev)
{
    char buf[BLOCK_SIZE];
    SUPER *sp;
    GD *gp;
    get_block(dev, 1, buf);
    sp = (SUPER *)buf;
    sp->s_free_blocks_count--;
    put_block(dev, 1, buf);
    get_block(dev, 2, buf);
    gp = (GD *)buf;
    gp->bg_free_blocks_count--;
    put_block(dev, 2, buf);
}

void incFreeBlocks(int dev)
{
    char buf[BLOCK_SIZE];
    SUPER *sp;
    GD *gp;
    get_block(dev, 1, buf);
    sp = (SUPER *)buf;
    sp->s_free_inodes_count++;
    put_block(dev, 1, buf);
    get_block(dev, 2, buf);
    gp = (GD *)buf;
    gp->bg_free_inodes_count++;
    put_block(dev, 2, buf);
}

int balloc(int dev, MINODE *root)
{
    int i, bmap;
    char buf[BLOCK_SIZE];
    MTABLE *mp = root->mptr;

    bmap = mp->bmap;

    get_block(dev, bmap, buf);
    for (i = 0; i < mp->nblock; i++)
    {
        if (test_bit(buf, i) == 0)
        {
            set_bit(buf, i);
            put_block(dev, bmap, buf);
            // Update free inode count in SUPER and GD
            decFreeBlocks(dev);
            return (i + 1);
        }
    }

    return 0;
}

void bdalloc(int dev, int bno, MINODE *root)
{
    int bmap;
    char buf[BLOCK_SIZE];
    MTABLE *mp = root->mptr;

    bmap = mp->bmap;

    if (bno > mp->nblock)
    {
        printf("Block number %d out of range\n", bno);
        return;
    }

    // Get INODE bitmap block
    get_block(dev, bmap, buf);
    clear_bit(buf, bno-1);

    // Write buffer back to block
    put_block(dev, bmap, buf);
    // Update free inode count in SUPER and GD
    incFreeBlocks(dev);
}

int search (int dev, MINODE *mip, char *name)
{
    int i, blockNo, i_blocks[15];
    DIR *tmp;
    char buf[BLOCK_SIZE], *cp;

    // loads all the blocks into an array
    for (i = 0; i < 15; i++)
    {
        i_blocks[i] = mip->INODE.i_block[i];
    }

    // Check direct entries
    for (i = 0; i < 12; i++)
    {
        blockNo = i_blocks[i];
        if (blockNo == 0)
        {
            break;
        }
        get_block(dev, blockNo, buf);
        tmp = (DIR *)buf;
        cp = buf;
        while (cp < buf + BLOCK_SIZE)
        {
            if (strcmp(name, tmp->name) == 0)
            {
                return tmp->inode;
            }
            cp += tmp->rec_len;
            tmp = (DIR *)cp;
        }
    }
    return 0;
}

int tokenize(char *pathname, char **names[])
{
    char *token, **tmp;
    int i = 0;
    size_t size = 0;

    *names = (char **)realloc(*names, sizeof(char *));

    if (pathname[0] == '/')
    {
        pathname++;
    }

    token = strtok(pathname, "/");
    while (token != NULL)
    {
        tmp = realloc(*names, size + sizeof(char *));
        if (tmp != NULL)
        {
            *names = tmp;
        }
        else
        {
            printf("Realloc failed.\n");
            return -1;
        }
        size += sizeof(char *);
        (*names)[i] = NULL;
        (*names)[i] = realloc(*names[i], sizeof(char) * (strlen(token) + 1));
        strcpy(*names[i], token);
        token = strtok(NULL, "/");
        i++;
    }

    return i;
}

int getino(char *pathname, MINODE *root, PROC *running, MINODE *minode)
{
    int i, n, ino, dev;
    char buf[BLOCK_SIZE], **names = NULL;
    MINODE *mip = NULL;

    dev = root->dev;

    // checks if it is just root
    if (strcmp(pathname, "/") == 0)
        return 2;

    // checks if it is an absolute path
    if (pathname[0] == '/')
        mip = iget(dev, 2, minode);
    else
        mip = iget(dev, running->cwd->ino, minode);

    strcpy(buf, pathname);
    n = tokenize(buf, &names);

    for (i = 0; i < n; i++)
    {
        ino = search(dev, mip, names[i]);

        // if inode does not exist the mip is written back to the disk
        if (ino == 0)
        {
            iput(mip, minode);
            return 0;
        }
        iput(mip, minode);
        mip = iget(dev, ino, minode);
    }
    iput(mip, minode);

    return ino;
}

int get_super_block(int dev, struct ext2_super_block **sb)
{
    char *buf = (char *)malloc(sizeof(char) * BLOCK_SIZE);
    int err = get_block(dev, 1, buf);

    *sb = (struct ext2_super_block *)buf;

    return err;
}

void print_super_block(int dev)
{
    struct ext2_super_block *sb;
    if (get_super_block(dev, &sb) < 0)
    {
        printf("Reading super block failed!\n");
        return;
    }

    printf("********* Super Block Info **********\n");
    printf("%-29s %5d\n", "inodes_count",       sb->s_inodes_count);
    printf("%-29s %5d\n", "blocks_count",       sb->s_blocks_count);
    printf("%-29s %5d\n", "r_blocks_count",     sb->s_r_blocks_count);
    printf("%-29s %5d\n", "free_blocks_count",  sb->s_free_blocks_count);
    printf("%-29s %5d\n", "free_inodes_count",  sb->s_free_inodes_count);
    printf("%-29s %5d\n", "log_block_size",     sb->s_log_block_size);
    printf("%-29s %5d\n", "first_data_block",   sb->s_first_data_block);
    printf("%-29s %x\n",  "magic_number",       sb->s_magic);
    printf("%-29s %5d\n", "revision_level",     sb-> s_rev_level);
    printf("%-29s %5d\n", "inode_size",         sb->s_inode_size);
    printf("%-29s %5d\n", "block_group_nr",     sb->s_block_group_nr);
    printf("%-29s %5d\n", "block_size",         1024 *
                                                (1 << sb->s_log_block_size));
    printf("%-29s %5d\n", "inodes_per_group",   sb->s_inodes_per_group);
    printf("-------------------------------------\n");
    printf("%-29s %5d\n", "desc_per_block",     32);
    printf("%-29s %5d\n", "inodes_per_block",   (1024 *
                                                (1 << sb->s_log_block_size)) /
                                                sb->s_inode_size);
    printf("%-29s %5d\n", "inodes_size_ratio",  1);
    printf("*************************************\n");
}

void print_group_info(int dev, int group_num, int first_data_block)
{
    struct ext2_group_desc *gd;
    char buf[BLOCK_SIZE];
    get_block(dev, first_data_block + 1 + group_num, buf);

    gd = (struct ext2_group_desc *)buf;

    printf("********** Group %d Info ***********\n", group_num);
    printf("%-29s %5d\n", "Blocks bitmap block",    gd->bg_block_bitmap);
    printf("%-29s %5d\n", "Inodes bitmap block",    gd->bg_inode_bitmap);
    printf("%-29s %5d\n", "Inodes table block",     gd->bg_inode_table);
    printf("%-29s %5d\n", "Free blocks count",      gd->bg_free_blocks_count);
    printf("%-29s %5d\n", "Free inodes count",      gd->bg_free_inodes_count);
    printf("%-29s %5d\n", "Used dirs count",        gd->bg_used_dirs_count);
    printf("***********************************\n");
}

void print_minode(MINODE *minode)
{
    printf("********** MINODE %d:%d Info **********\n", minode->dev, minode->ino);
    print_inode(&minode->INODE);
    printf("%-29s %5d\n", "Dev",                    minode->dev);
    printf("%-29s %5d\n", "Ino",                    minode->ino);
    printf("%-29s %5d\n", "RefCount",               minode->refCount);
    printf("%-29s %5d\n", "Dirty",                  minode->dirty);
    printf("%-29s %5d\n", "Mounted",                minode->mounted);
    printf("%-29s    \n", "Mount Table:"            );
    printf("%-29s %5d\n", "    tnblock",               minode->mptr->nblock);
    printf("%-29s %5d\n", "    ninodes",              minode->mptr->ninodes);
    printf("%-29s %5d\n", "    bmap",                 minode->mptr->bmap);
    printf("%-29s %5d\n", "    imap",                 minode->mptr->imap);
    printf("%-29s %5d\n", "    iblock",               minode->mptr->iblock);
    printf("***************************************\n");
}

void print_inode(INODE *inode)
{
    printf("%-29s %5d\n", "Mode",                   inode->i_mode);
    printf("%-29s %5d\n", "UID",                    inode->i_uid);
    printf("%-29s %5d\n", "Size",                   inode->i_size);
    printf("%-29s %5d\n", "CTime",                  inode->i_ctime);
    printf("%-29s %5d\n", "Links",                  inode->i_links_count);
}

void init(PROC *p[], MINODE *minode[], int numMInodes, MINODE **root)
{
    int i = 0;

    // allocating memory for the processes
    *p = (PROC *)(malloc(2 * sizeof(PROC)));

    // setting the uids for each process
    (*p)[0].uid = 0;
    (*p)[0].gid = 0;

    // Get the uid and gid of the user
    (*p)[1].uid = geteuid();
    (*p)[1].gid = getegid();

    // setting the cwd of each process to 0
    (*p)[0].cwd = NULL;
    (*p)[1].cwd = NULL;

    // setting all of the refCount of the minode to zero
    *minode = (MINODE *)malloc((numMInodes + 1) * sizeof(MINODE));

    for (i = 0; i < numMInodes; i++)
    {
        (*minode)[i].refCount = 0;
    }

    //Use dev = -1 to denote end of array
    (*minode)[numMInodes].dev = -1;

    // setting root to NULL
    *root = NULL;
}

int mount_root(char *devName, MINODE **root, MINODE *minode, PROC **running, PROC *p[], struct mntTable **mtables)
{
    char buf[BLKSIZE];
    int fd = 0;
    SUPER *sp;
    GD *gp;
    struct mntTable *mtable;

    // opening the device for read and write mode
    fd = open(devName, O_RDWR);
    if (fd < 0)
    {
        printf("Open %s failed\n", devName);
        return -1;
    }

    // reads the SUPER block
    get_block(fd, 1, buf);
    sp = (SUPER *)buf;

    // checks if it is a valid device
    if (sp->s_magic != 0xEF53)
    {
        printf("%s is NOT an EXT2 FS\n", devName);
        return 1;
    }

    // get the root inode
    *root = iget(fd, 2, minode);

    // Make sure that the mount tables are allocated properly
    if (*mtables == NULL)
    {
        *mtables = (struct mntTable *)malloc(sizeof(struct mntTable));
    }

    mtable = *mtables;

    // using mTable[0] to record
    // dev = fd
    mtable[0].dev = fd;

    // ninodes, nblocks from superblock
    mtable[0].ninodes = sp->s_inodes_count;
    mtable[0].nblock = sp->s_blocks_count;

    // reads the group desc block
    get_block(fd, 2, buf);
    gp = (GD *)buf;

    // bmap, imap, iblock from GD
    mtable[0].bmap = gp->bg_block_bitmap;
    mtable[0].imap = gp->bg_inode_bitmap;
    mtable[0].iblock = gp->bg_inode_table;

    // mount point DIR pointer = root
    mtable[0].mountDirPtr = *root;

    // device name = "YOUR DISK name"
    strcpy(mtable[0].devName, devName);

    // mntPointDirName = "/"
    strcpy(mtable[0].mntName, "/");

    // set cwd of p0 and p1 point at the root minode
    (*p)[0].cwd = iget(fd, 2, minode);
    (*p)[1].cwd = iget(fd, 2, minode);

    // Assign root's mptr to mtable[0]
    (*root)->mptr = mtable;

    // let running->p0
    *running = p[0];

    return fd;
}
