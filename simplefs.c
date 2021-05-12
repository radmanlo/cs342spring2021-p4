#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include "simplefs.h"

//Superblock
struct superBlock{
    int numBlocks;
    int totalVolume;
    int blockSize;
};

//initializing the superblock
void initSupBlock(int diskSize);

//BitMapBlocks
// https://stackoverflow.com/questions/44978126/structure-for-an-array-of-bits-in-c
struct bitMapBlocks{
    unsigned char bitMap[BLOCKSIZE];
};

//intializing the bitmap blocks
void initBitMap();

//set the bit to 1
void setBit(int index, unsigned char* bitMap);

//set the bit to 0
void clearBit(int index, unsigned char* bitMap);

//read the bit
int readBit(int index, unsigned char* bitMap);

//Directory Blocks
struct dirBlock{
    char directories[32][110];
    int iNodeFcb[32];
};

//initializing directory blocks
void initDirBlocks();

//Fcb blocks
struct fcbBlock{
    bool used[32]; // 1 for used, 0 for not used
    int indexBlock[32];
    int sizeOfFile[32];
};

void initFcbBlocks();

// Global Variables =======================================
int vdisk_fd; // Global virtual disk file descriptor. Global within the library.
              // Will be assigned with the vsfs_mount call.
              // Any function in this file can use this.
              // Applications will not use  this directly. 
// ========================================================


// read block k from disk (virtual disk) into buffer block.
// size of the block is BLOCKSIZE.
// space for block must be allocated outside of this function.
// block numbers start from 0 in the virtual disk. 
int read_block (void *block, int k)
{
    int n;
    int offset;

    offset = k * BLOCKSIZE;
    lseek(vdisk_fd, (off_t) offset, SEEK_SET);
    n = read (vdisk_fd, block, BLOCKSIZE);
    if (n != BLOCKSIZE) {
	    printf ("read error\n");
	return -1;
    }
    return (0); 
}

// write block k into the virtual disk. 
int write_block (void *block, int k)
{
    int n;
    int offset;

    offset = k * BLOCKSIZE;
    lseek(vdisk_fd, (off_t) offset, SEEK_SET);
    n = write (vdisk_fd, block, BLOCKSIZE);
    if (n != BLOCKSIZE) {
        printf ("write error\n");
        return (-1);
    }
    return 0; 
}


/**********************************************************************
   The following functions are to be called by applications directly. 
***********************************************************************/

// this function is partially implemented.
int create_format_vdisk (char *vdiskname, unsigned int m)
{
    char command[1000];
    int size;
    int num = 1;
    int count;
    size  = num << m;
    count = size / BLOCKSIZE;
    //    printf ("%d %d", m, size);
    sprintf (command, "dd if=/dev/zero of=%s bs=%d count=%d",
             vdiskname, BLOCKSIZE, count);
    //printf ("executing command = %s\n", command);
    if (system (command) == -1) {
         printf("error in creating file n create_format_vdisk function");   
         return (-1);

    }

    //the code to format the disk below.
    sprintf (command, "mkfs %s", vdiskname);
    //system git to the shall
    if (system (command) == -1) {
         printf("error in formating file in create_format_vdisk function");   
         return (-1);

    }
    sfs_mount(vdiskname);
    initSupBlock(size);
    printf("superblock is initialized\n");
    initBitMap();
    printf("bitmap is initialized\n");
    initDirBlocks();
    printf("directory is initialized\n");
    initFcbBlocks();
    printf("FCB block is initialized\n");
    sfs_umount();
    return (0); 
}


// already implemented
int sfs_mount (char *vdiskname)
{
    // simply open the Linux file vdiskname and in this
    // way make it ready to be used for other operations.
    // vdisk_fd is global; hence other function can use it. 
    vdisk_fd = open(vdiskname, O_RDWR); 
    if (vdisk_fd == -1){
        printf("error in opening file in sfs_mount function");
        return(-1);
    }
    return(0);
}


// already implemented
int sfs_umount ()
{
    // copy everything in memory to disk
    if (fsync (vdisk_fd) == -1){
        printf("error in copy file in sfs_unmount function");
        return(-1);
    }
    if (close (vdisk_fd) == -1){
        printf("error in colse file in sfs_unmount function");
        return(-1);
    }
    return (0); 
}


int sfs_create(char *filename)
{
    for(int bitBlockIndex = 1; bitBlockIndex < 5; bitBlockIndex++){
        struct bitMapBlocks* bBlock;
        read_block(bBlock,bitBlockIndex);

        for(int i = 0 ; i < 32767; i++){
            if(readBit(i,bBlock->bitMap) == 0){
                setBit(i, bBlock->bitMap); // Allocated for index Block
                write_block(bBlock, bitBlockIndex);
                struct dirBlock* dBlock;
                for(int dirBlockIndex = 5; dirBlockIndex < 9; dirBlockIndex++){
                    read_block(dBlock, dirBlockIndex);
                    for(int j = 0; j < 32; j++){
                        if(dBlock->iNodeFcb[j] == -1){
                            dBlock->iNodeFcb[j] = j;
                            struct fcbBlock* fBlock;
                            int fBlockIndex = dirBlockIndex + 4;
                            read_block(fBlock, fBlockIndex);
                            fBlock->sizeOfFile[j] = 0;
                            fBlock->used[j] = 0;
                            fBlock->indexBlock[j] = ((bitBlockIndex-1) * 32767) + i;
                            write_block(dBlock, dirBlockIndex);
                            write_block(fBlock, fBlockIndex);
                            printf("Successfully created file\n");
                            return (0);
                        }
                    }
                }
            }
        }
    }
    printf("Could not create file\n");
    return (-1);
}


int sfs_open(char *file, int mode)
{
    return (0); 
}

int sfs_close(int fd){
    return (0); 
}

int sfs_getsize (int  fd)
{
    return (0); 
}

int sfs_read(int fd, void *buf, int n){
    return (0); 
}


int sfs_append(int fd, void *buf, int n)
{
    return (0); 
}

int sfs_delete(char *filename)
{
    return (0); 
}

void initSupBlock(int diskSize){
    struct superBlock* supBlock;
    supBlock = (struct superBlock*) malloc (sizeof(struct superBlock));
    supBlock->blockSize = BLOCKSIZE;
    supBlock->numBlocks = diskSize / BLOCKSIZE;
    supBlock->totalVolume = diskSize;
    write_block(supBlock, 0 );
    free(supBlock);
};

void initBitMap(){
    struct bitMapBlocks* bBlock;
    bBlock = (struct bitMapBlocks*) malloc(sizeof (struct bitMapBlocks));
    for(int i = 0 ; i < 32767; i++){
        clearBit(i, bBlock->bitMap); // it is 0 hence ready to use
    }
    write_block(bBlock, 2);
    write_block(bBlock, 3);
    write_block(bBlock, 4);
    for(int i = 0; i < 13; i++){
       setBit(i, bBlock->bitMap); // these bits are 1. are already to use
    }
    write_block(bBlock, 1);
    free(bBlock);
};

void setBit(int index, unsigned char* bitMap){
    bitMap[index/8] |= (1 << (index%8));
}

void clearBit(int index, unsigned char* bitMap){
    bitMap[index/8] &= ~(1 << (index%8));
}

int readBit(int index, unsigned char* bitMap){
    int result = (bitMap[index/8] &= (1 << (index%8)) != 0);
    return result;
}


void initDirBlocks(){
    struct dirBlock* dBlock;
    dBlock = (struct dirBlock*) malloc(sizeof (struct  dirBlock));
    for(int i = 0; i < 32; i++){
        dBlock->iNodeFcb[i] = -1;
    }
    write_block(dBlock, 5);
    write_block(dBlock, 6);
    write_block(dBlock, 7);
    write_block(dBlock, 8);
    free(dBlock);
}

void initFcbBlocks(){
    struct fcbBlock* fBlock;
    fBlock = (struct fcbBlock*) malloc(sizeof(struct  fcbBlock));
    for (int i = 0; i < 32; i++) {
        fBlock->indexBlock[i] = -1;
        fBlock->used[i] = false;
        fBlock->sizeOfFile[i] = -1;
    }
    write_block(fBlock, 9);
    write_block(fBlock, 10);
    write_block(fBlock, 11);
    write_block(fBlock, 12);
    free(fBlock);
};
