#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "simplefs.h"

int main(int argc, char **argv)
{
    clock_t start;
    clock_t stop;
    double elapsed;
    int ret;
    int fd1, fd2, fd;
    int i;
    char c;
    char buffer[1024];
    char buffer2[8] = {50, 50, 50, 50, 50, 50, 50, 50};
    int size;
    char vdiskname[200];

    printf ("started\n");

    if (argc != 2) {
        printf ("usage: app  <vdiskname>\n");
        exit(0);
    }
    strcpy (vdiskname, argv[1]);

    ret = sfs_mount (vdiskname);
    if (ret != 0) {
        printf ("could not mount \n");
        exit (1);
    }

    printf ("creating files\n");
    sfs_create ("file1.bin");
    sfs_create ("file2.bin");
    sfs_create ("file3.bin");
    
    /*start = clock();
    sfs_create ("file1.bin");
    stop = clock();
    elapsed = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
    printf("Time elapsed in ms: %f\n", elapsed * 1000000);
    start = clock();
    sfs_create ("file2.bin");
    stop = clock();
    elapsed = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
    printf("Time elapsed in ms: %f\n", elapsed * 1000000);
    start = clock();
    sfs_create ("file3.bin");
    stop = clock();
    elapsed = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
    printf("Time elapsed in ms: %f\n", elapsed * 1000000);
    start = clock();
    sfs_create ("file4.bin");
    stop = clock();
    elapsed = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
    printf("Time elapsed in ms: %f\n", elapsed * 1000000);
    start = clock();
    sfs_create ("file5.bin");
    stop = clock();
    elapsed = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
    printf("Time elapsed in ms: %f\n", elapsed * 1000000);
    start = clock();
    sfs_create ("file6.bin");
    stop = clock();
    elapsed = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
    printf("Time elapsed in ms: %f\n", elapsed * 1000000);*/


    start = clock();
    fd1 = sfs_open ("file1.bin", MODE_APPEND);
    stop = clock();
    elapsed = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
    printf("Time elapsed in ms: %f\n", elapsed * 1000000);
    fd2 = sfs_open ("file2.bin", MODE_APPEND);

    start = clock();
    sfs_close(fd1);
    stop = clock();
    elapsed = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;

    fd1 = sfs_open ("file1.bin", MODE_APPEND);

    for (i = 0; i < 10000; ++i) {
        buffer[0] =   (char) 65;
        sfs_append (fd1, (void *) buffer, 1);
    }

    for (i = 0; i < 10000; ++i) {
        buffer[0] = (char) 65;
        buffer[1] = (char) 66;
        buffer[2] = (char) 67;
        buffer[3] = (char) 68;
        sfs_append(fd2, (void *) buffer, 4);
    }

    sfs_close(fd1);
    sfs_close(fd2);

    fd = sfs_open("file3.bin", MODE_APPEND);
    for (i = 0; i < 10000; ++i) {
        memcpy (buffer, buffer2, 8); // just to show memcpy
        sfs_append(fd, (void *) buffer, 8);
    }
    sfs_close (fd);

    fd = sfs_open("file3.bin", MODE_READ);
    size = sfs_getsize (fd);
    for (i = 0; i < size; ++i) {
        sfs_read (fd, (void *) buffer, 1);
        c = (char) buffer[0];
        c = c + 1;
    }
    sfs_close (fd);
    ret = sfs_umount();


    /*clock_t start = clock();
    // Execuatable code
    clock_t stop = clock();
    double elapsed = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
    printf("Time elapsed in ms: %f", elapsed);*/
}