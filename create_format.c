#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "simplefs.h"



void Bit(int index, int* bits){
    int loc = index / 8;
    int value = 1;
    int temp;
    temp = (index % 8); 
    temp = (8 - temp) - 1;
    printf("temp %d\n", temp);
    for (int a = 0; a < temp; a++){
        value = value * 10;
    }
    printf("value %d temp %d\n", value, temp);
    bits[loc] = bits[loc] + value;
}

void clrBit(int index, int* bits){
    int loc = index / 8;
    int value = 1;
    int temp;
    temp = (index % 8); 
    temp = (8 - temp) - 1;
    printf("temp %d\n", temp);
    for (int a = 0; a < temp; a++){
        value = value * 10;
    }
    printf("value %d temp %d\n", value, temp);
    bits[loc] = bits[loc] - value;   
}

int readb(int index, int* bits){
    int loc = index / 8;
    int value = 1;
    int temp;
    double temp1 = 0;
    double temp2 = 0;
    temp = (index % 8); 
    temp = (8 - temp) - 1;
    for (int a = 0; a < temp; a++){
        value = value * 10;
    }
    if (bits[loc] < value) 
        return 0;
    else{
        temp1 = (double)bits[loc] / (double)value;
        printf("temp1 %f\n", temp1);
        temp1 = temp1 - 0.1;
        value = bits[loc] / value;
        printf("temp1 %f\n", temp1);
        temp2 = (double) value;
        printf("temp1 %f- temp2 %f\n", temp1, temp2);
        if (temp1 >= temp2 || temp1 == 0)
            return 1;
        else
            return 0;
    } 
    return -1;
}


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

    printf ("creating files\n"); 
    sfs_create ("file1.bin");
    ret = sfs_umount();

    
   
    int* bits = malloc(1024 * sizeof(int));
    for (int i =0; i < 1024; i++)
        bits[i] = 0;
    Bit(0, bits);
    printf("%d\n", bits[0]);
    printf("%d\n", bits[1]);
    Bit(1, bits);
    printf("%d\n", bits[0]);
    printf("%d\n", bits[1]);
    Bit(2, bits);
    printf("%d\n", bits[0]);
    printf("%d\n", bits[1]);
    Bit(3, bits);
    printf("%d\n", bits[0]);
    printf("%d\n", bits[1]);
    Bit(4, bits);
    printf("%d\n", bits[0]);
    printf("%d\n", bits[1]);
    Bit(5, bits);
    printf("%d\n", bits[0]);
    printf("%d\n", bits[1]);
    Bit(6, bits);
    printf("%d\n", bits[0]);
    printf("%d\n", bits[1]);
    Bit(7, bits);
    printf("%d\n", bits[0]);
    printf("%d\n", bits[1]);
    Bit(8, bits);
    printf("%d\n", bits[0]);
    printf("%d\n", bits[1]);
    Bit(9, bits);
    printf("%d\n", bits[0]);
    printf("%d\n", bits[1]);
    Bit(10, bits);
    printf("%d\n", bits[0]);
    printf("%d\n", bits[1]);
    Bit(11, bits);
    printf("%d\n", bits[0]);
    printf("%d\n", bits[1]);

    printf("\n");
    /*clrBit(0, bits);
    printf("%d\n", bits[0]);
    printf("%d\n", bits[1]);
    clrBit(1, bits);
    printf("%d\n", bits[0]);
    printf("%d\n", bits[1]);
    clrBit(2, bits);
    printf("%d\n", bits[0]);
    printf("%d\n", bits[1]);
    clrBit(3, bits);
    printf("%d\n", bits[0]);
    printf("%d\n", bits[1]);
    clrBit(4, bits);
    printf("%d\n", bits[0]);
    printf("%d\n", bits[1]);
    clrBit(5, bits);
    printf("%d\n", bits[0]);
    printf("%d\n", bits[1]);
    clrBit(6, bits);
    printf("%d\n", bits[0]);
    printf("%d\n", bits[1]);
    clrBit(7, bits);
    printf("%d\n", bits[0]);
    printf("%d\n", bits[1]);
    clrBit(8, bits);
    printf("%d\n", bits[0]);
    printf("%d\n", bits[1]);
    clrBit(9, bits);
    printf("%d\n", bits[0]);
    printf("%d\n", bits[1]);
    clrBit(10, bits);
    printf("%d\n", bits[0]);
    printf("%d\n", bits[1]);
    clrBit(11, bits);
    printf("%d\n", bits[0]);
    printf("%d\n", bits[1]);*/

    printf("read %d\n", readb(0,bits));
    printf("read %d\n", readb(8,bits));
    printf("read %d\n", readb(10,bits));
    printf("read %d\n", readb(3,bits));
    printf("read %d\n", readb(9,bits));
    printf("read %d\n", readb(25,bits));

   /* double a = (double)11101 / 1000;

    printf("a %f\n", a);*/
    /*int output[8];
    char c = -1;
    char a[8];
    char b[8];
    int i;
    for (i = 0; i < 8; ++i) {
        output[i] = (c >> i) & 1;
        //strncat(a, (char*)((c >> i) & 1), 1);
    }
    for(int m = 0; m<8; m++){
        printf("%d ", output[m]);
    }
    printf("\n");
    
    for(int m = 0; m<8; m++){
        //a = output[m] + '0';
        sprintf(a, "%d ", output[m]);
        strncat(b, a, 1);
    }
    
    //sprintf(a, "%d", output);
    printf("%s\n ",b);
    strcpy(c, b);
    printf("%c\n ",c);
    for (i = 0; i < 8; ++i) {
        output[i] = (b[i] >> i) & 1;
        //strncat(a, (char*)((c >> i) & 1), 1);
    }
    for(int m = 0; m<8; m++){
        printf("%d ", output[m]);
    }
    return(0);*/
}
