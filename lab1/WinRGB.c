
// helper program, convert 24 bit RGB to 16 bit X style

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void printBin(unsigned char IN, int size);

int main (int argc, char **argv)
{
    unsigned char pixIN[3], pixOUT;
    unsigned char dispData[2];

    pixIN[0] = 253;
    pixIN[1] = 129;
    pixIN[2] = 135;

    printf("RED value: %d\n",pixIN[0]);
    printf("RED binary: ");
    printBin(pixIN[0], 8);
    printf("\n");

     printf("GREEN value: %d\n",pixIN[1]);
    printf("GREEN binary: ");
    printBin(pixIN[1], 8);
    printf("\n");

    printf("BLUE value: %d\n",pixIN[2]);
    printf("BLUE binary: ");
    printBin(pixIN[2], 8);
    printf("\n");

    dispData[0] = 0x00;
    dispData[1] = 0x00;

    unsigned char tmp = 0x00;
    tmp = pixIN[0] & 0xF8;

    //get red
    tmp = pixIN[0] & 0xF8;  // first 5 bits of RED;
    dispData[0] = dispData[0] | tmp;

    //get green
    tmp = pixIN[1] & 0xF8;  // first 5 bits of GREEN;
    dispData[0] |= tmp>>5;
    dispData[1] |= tmp<<4;

    //get blue
    tmp = pixIN[2] & 0xF8;  //first 5 bits of BLUE;
    dispData[1] |= tmp >> 2;


    printf("dispData[0]: %d\n", dispData[0]);

    printf("dispData[1]: %d\n", dispData[1]);

    printf("dispData: ");
    printBin(dispData[0], 8);
    printBin(dispData[1], 8);
    printf("\n");

    return 0;
}




void printBin(unsigned char IN, int size) 
{
    int i;

    for(i = size-1; i >=0; i--) {
        printf("%c", (IN & (1<<i)) ? '1' : '0');
    }
}


















