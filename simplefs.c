#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>
#include "simplefs.h"

//Global cursor to read
int cursor = 0;
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
    int bitMap[1024];
};

//intializing the bitmap blocks
void initBitMap();

//set the bit to 1
void setBit(int index, int* bitMap);

//set the bit to 0
void clearBit(int index, int* bitMap);

//read the bit
int readBit(int index, int* bitMap);

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
    int mode[32]; // 0 FOR READ ONLY | 1 FOR WRITE ONLY |
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
    initBitMap(count);
    printf("bitmap is initialized\n");
    initDirBlocks();
    printf("directory is i nitialized\n");
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
        bBlock = (struct bitMapBlocks*) malloc(sizeof (struct bitMapBlocks));
        read_block(bBlock,bitBlockIndex);
        printf("I read here\n");
        clearBit(15, bBlock->bitMap);
        printf("bitMap %d\n", bBlock->bitMap[0]);
        printf("bitMap %d\n", bBlock->bitMap[1]);
        for(int i = 0 ; i < 1024; ++i){
            if(readBit(i, bBlock->bitMap) == 0){
                printf("Bitmap found availailable at i = %d, in block index = %d", i, bitBlockIndex);
                setBit(i, bBlock->bitMap); // Allocated for index Block
                write_block(bBlock, bitBlockIndex);
                struct dirBlock* dBlock;
                dBlock = (struct dirBlock*) malloc(sizeof (struct  dirBlock));
                for(int dirBlockIndex = 5; dirBlockIndex < 9; dirBlockIndex++){
                    read_block(dBlock, dirBlockIndex);
                    for(int j = 0; j < 32; j++){
                        if(dBlock->iNodeFcb[j] == -1){
                            dBlock->iNodeFcb[j] = j;
                            strncpy(dBlock->directories[i], filename, 110);
                            struct fcbBlock* fBlock;
                            fBlock = (struct fcbBlock*) malloc(sizeof(struct  fcbBlock));
                            int fBlockIndex = dirBlockIndex + 4;
                            read_block(fBlock, fBlockIndex);
                            fBlock->sizeOfFile[j] = 0;
                            fBlock->used[j] = 0;
                            fBlock->indexBlock[j] = ((bitBlockIndex-1) * 32767) + i;
                            printf("Indexed Allocated Block is = %d", fBlock->indexBlock[j]);
                            write_block(dBlock, dirBlockIndex);
                            write_block(fBlock, fBlockIndex);
                            printf("dirBlock av inode found at %d, at %d th block of dirblock ", j, dirBlockIndex);
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
    struct dirBlock* db = (struct dirBlock*) malloc(sizeof (struct dirBlock));
    for(int i = 5; i < 9; i++){
        read_block(db, i);
        for(int j = 0; j < 32; i++){
            char* res = strstr(db->directories[j], file);
            if(res){
                struct fcbBlock* fBlock;
                fBlock = (struct fcbBlock*) malloc(sizeof(struct  fcbBlock));
                read_block(fBlock, i+4);
                if(fBlock->used[j] == 1){
                    printf("File is opened by another process please try later.\n");
                    return(-1);
                }
                fBlock->used[j] = 1;
                fBlock->mode[j] = mode;
                int rFd = (((i-5)*32) + j);
                printf("File found in sfs_open function. Fd index = %d\n", rFd);
                return rFd; // indicates which file it is
            }
        }
    }
    printf("Error occured in sfs_open, couldn't find file.\n");
    return (-1);
}

int sfs_close(int fd){
    if(fd>127 || fd <0){
        printf("There is not such available fd \n");
        return(-1);
    }
    // Computes quotient
    int quotient = fd / 32; // INDICATES WHICH DIRECTORY BLOCK
    // Computes remainder
    int remainder = fd % 32; // INDICATES WHICH INDEX OF DIRECTORY BLOCK
    struct fcbBlock* fBlock;
    fBlock = (struct fcbBlock*) malloc(sizeof(struct  fcbBlock));
    read_block(fBlock, quotient+5+4);
    fBlock->mode[remainder] = -1;
    fBlock->used[remainder] = 0; // Free to use now;
    printf("File is closed! \n");
    return (0); 
}

int sfs_getsize (int  fd)
{
    // Computes quotient
    int quotient = fd / 32; // INDICATES WHICH DIRECTORY BLOCK
    // Computes remainder
    int remainder = fd % 32; // INDICATES WHICH INDEX OF DIRECTORY BLOCK
    struct fcbBlock* fBlock;
    fBlock = (struct fcbBlock*) malloc(sizeof(struct  fcbBlock));
    read_block(fBlock, quotient+5+4);
    return fBlock->sizeOfFile[remainder];
}

int sfs_read(int fd, void *buf, int n){
    // Computes quotient
    int quotient = fd / 32; // INDICATES WHICH DIRECTORY BLOCK
    // Computes remainder
    int remainder = fd % 32; // INDICATES WHICH INDEX OF DIRECTORY BLOCK
    struct fcbBlock* fBlock;
    fBlock = (struct fcbBlock*) malloc(sizeof(struct  fcbBlock));
    read_block(fBlock, quotient+5+4);
    if(fBlock->sizeOfFile[remainder] >= 0){
        printf("No available data in fd to read (sfs_read)");
        return -1;
    }
    if(fBlock->mode[remainder] != MODE_READ){
        printf("Fd is not available in READ MODE now.");
        return (-1);
    }
    int sizeOfFile = fBlock->sizeOfFile[remainder];
    int whichBlock = cursor / 4096;
    //if(cursor % BLOCKSIZE)
    int* blockIndexes = (int*) malloc(BLOCKSIZE / 4);
    read_block(blockIndexes, fBlock->indexBlock[remainder]);
    char* blockData = (char*) malloc(sizeof (BLOCKSIZE));
    read_block(blockData, blockIndexes[whichBlock]);
    ((char*)buf)[0] = blockData[cursor];
    cursor = cursor + n;
    if(cursor >= sizeOfFile-n){
        cursor = 0;
    }
    return (0);
}


int sfs_append(int fd, void *buf, int n)
{
    //TODO
    // Computes quotient
    int quotient = fd / 32; // INDICATES WHICH DIRECTORY BLOCK
    // Computes remainder
    int remainder = fd % 32; // INDICATES WHICH INDEX OF DIRECTORY BLOCK
    struct fcbBlock* fBlock;
    fBlock = (struct fcbBlock*) malloc(sizeof(struct  fcbBlock));
    read_block(fBlock, quotient+5+4);
    return (0); 
}

int sfs_delete(char *filename)
{
    struct dirBlock* db = (struct dirBlock*) malloc(sizeof (struct dirBlock));
    for(int i = 5; i < 9; i++){
        read_block(db, i);
        for(int j = 0; j < 32; j++){
            char* res = strstr(db->directories[j], filename);
            if(res){
                if(db->iNodeFcb[j] != -1) {
                    db->iNodeFcb[j] = -1;
                    write_block(db,i);
                    struct fcbBlock *fBlock;
                    fBlock = (struct fcbBlock *) malloc(sizeof(struct fcbBlock));
                    read_block(fBlock, i + 4);

                    int *blockIndexes = (int *) malloc(BLOCKSIZE / 4);
                    read_block(blockIndexes, fBlock->indexBlock[j]);
                    fBlock->sizeOfFile[j] = 0;
                    write_block(fBlock, i+4);
                    for (int k = 0; k < 1024; k++) {
                        if (blockIndexes[k] == -1) {
                            k = 1025; // TYPE OF BREAK OF FOR LOOP
                        }else{
                            // Computes quotient
                        int quotient = blockIndexes[k] / 4096; // INDICATES WHICH DIRECTORY BLOCK
                        // Computes remainder
                        int remainder = blockIndexes[k] % 4096;
                        // CLEAR BITS TO NOT USED AFTER IMPLEMENTING BIT ARRAY.
                        //TODO
                        }
                    }
                }
            }
        }
    }
    printf("Error occured in sfs_open, couldn't find file.\n");
    return (-1);
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

void initBitMap(int blockCount){
    printf("initialize starts\n");
    struct bitMapBlocks* bBlock;
    bBlock = (struct bitMapBlocks*) malloc(sizeof (struct bitMapBlocks));
    for(int i = 0; i < 13; i++){
        setBit(i, bBlock->bitMap); // these bits are 1. are already used
    }
    if(blockCount < 4096){
        for(int i = 13; i < blockCount; i++ ){
            clearBit(i, bBlock->bitMap);  // Ready to use -0
        }
        for(int i = blockCount; i < 4096; i++){
            setBit(i, bBlock->bitMap);
        }
        write_block(bBlock,1);
        for(int i = 0 ; i < 4096; i++){
            setBit(i,bBlock->bitMap);
        }
        write_block(bBlock, 2);
        write_block(bBlock, 3);
        write_block(bBlock, 4);
    }else{
        for(int i = 13; i < 4096; i++){
            clearBit(i, bBlock->bitMap);  // Ready to use -0
        }
        write_block(bBlock,1);
        int remainingBlock = blockCount - 4096;
        for(int i = 2; i<5; i++){
            for(int l = 0; l < 4096; l++){
                clearBit(l, bBlock->bitMap);  // CLEAR BLOCK FOR OTHERS
            }
            for(int j = 0 ; j < 4096; j++){
                if(remainingBlock > 0){
                    clearBit(j,bBlock); // AVAILABLE
                    remainingBlock--;
                }else{
                    setBit(j, bBlock); // NOT AVAILABLE
                }
            }
            write_block(bBlock, i);
        }
    }
    free(bBlock);
};

void setBit(int index, int* bitMap){
    if (readBit(index, bitMap) != 0 ){
        printf("this index is full\n");
        return ;
    }
    int loc = index / 8;
    int value = 1;
    int temp;
    temp = (index % 8); 
    temp = (8 - temp) - 1;
    for (int a = 0; a < temp; a++){
        value = value * 10;
    }
    bitMap[loc] = bitMap[loc] + value;
}

void clearBit(int index, int* bitMap){
    if (readBit(index, bitMap) != 1 ){
        printf("this index is empty\n");
        return;
    }
    int loc = index / 8;
    int value = 1;
    int temp;
    temp = (index % 8); 
    temp = (8 - temp) - 1;
    for (int a = 0; a < temp; a++){
        value = value * 10;
    }
    bitMap[loc] = bitMap[loc] - value;  
}

int readBit(int index, int* bitMap){
    int loc = index / 8;
    int value = 1;
    int temp;
    double temp1 = 0;
    double temp2 = 0;
    temp = (index % 8); 
    temp = (8 - temp);
    for (int a = 0; a < temp; a++){
        value = value * 10;
    }
    temp1 = (double)bitMap[loc] / (double)value;
    temp1 = temp1 - 0.1;
    value = bitMap[loc] / value;
    temp2 = (double) value;
    if (temp1 >= temp2 || temp1 == 0)
        return 1;
    else
        return 0;
    return -1;
}

void initDirBlocks(){
    struct dirBlock* dBlock;
    dBlock = (struct dirBlock*) malloc(sizeof (struct  dirBlock));
    for(int i = 0; i < 32; i++){
        dBlock->iNodeFcb[i] = -1;
        //strncpy(dBlock->directories[i], '\0', 110);
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
        fBlock->mode[i] = -1;
    }
    write_block(fBlock, 9);
    write_block(fBlock, 10);
    write_block(fBlock, 11);
    write_block(fBlock, 12);
    free(fBlock);
};
