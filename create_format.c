#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "simplefs.h"

int main(int argc, char **argv)
{
    int ret;
    char vdiskname[200];
    int m; 

    if (argc != 3) {
	printf ("usage: create_format <vdiskname> <m>\n"); 
	exit(1); 
    }

    strcpy (vdiskname, argv[1]); 
    m = atoi(argv[2]); 
    
    printf ("started\n"); 
    
    ret  = create_format_vdisk (vdiskname, m); 
    if (ret != 0) {
        printf ("there was an error in creating the disk\n");
        exit(1); 
    }

    printf ("disk created and formatted. %s %d\n", vdiskname, m); 

    ret = sfs_mount (vdiskname);
    if (ret != 0) {
        printf ("could not mount \n");
        exit (1);
    }
    printf ("..........................CREATE............................................\n");

    printf ("creating files\n"); 
    sfs_create ("file1.bin");
    sfs_create ("file2.bin");
    sfs_create ("file3.bin");
    sfs_create ("file4.bin");
    sfs_create ("file5.bin");
    sfs_create ("file6.bin");
    printf ("..........................DELETE............................................\n");

    sfs_delete("file5.bin");
    printf ("\n"); 
    printf ("..........................OPEN..........................................\n");
    int fd = sfs_open("file1.bin", 0);
    int fd1 = sfs_open("file4.bin", 0);

    sfs_open("file1dwad.bin", 0);
    printf("size of size = %d\n", sfs_getsize(fd));
    printf("size of size = %d\n", sfs_getsize(fd1));
    ret = sfs_umount();
}
