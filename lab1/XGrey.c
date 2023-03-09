// helper program, convert 8 bit greyscale to 16 bit X style

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void printBin(unsigned char* IN, int size);

int main (int argc, char **argv)
{
    unsigned char pixIN, pixOUT;
    unsigned char dispData[2];

    pixIN = 133;

    printf("pixIN value: %d\n",pixIN);
    printf("pixIN binary: ");
    printBin(&pixIN, 8);
    printf("\n");

    dispData[0] = 0x00;
    dispData[1] = 0x00;
    
    unsigned char tmp = 0x0000;
    tmp = pixIN & 0xF8;
    unsigned char new;

    printf("tmp binary: ");
    printBin(&tmp, 8);
    printf("\n");
    
    dispData[0] = dispData[0] | tmp;
    dispData[1] = dispData[1] | (tmp >> 3);
    tmp = pixIN & 0xFC;
    dispData[0] = dispData[0] | (tmp >> 5);
    dispData[1] = dispData[1] | (tmp << 3);


    printf("dispData[0]: ");
    printBin(&dispData[0], 8);
    printf("\n");

    printf("dispData[1]: ");
    printBin(&dispData[1], 8);
    printf("\n");

    printf("dispData: ");
    printBin(&dispData[0], 8);
    printBin(&dispData[1], 8);
    printf("\n");

    return 0;
}




void printBin(unsigned char* IN, int size) 
{
    int i;

    for(i = size-1; i >=0; i--) {
        printf("%c", (*IN & (1<<i)) ? '1' : '0');
    }
}
