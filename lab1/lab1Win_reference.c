/*  Cameron Burroughs | C17134631 | burrou5@clemson.edu
 *  ECE 6680 | lab1 - Image Diaplay | Dr. Adam Hoover
 *  lab1.c 
 *  Description: program to load in ppm pixel data
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <windows.h>

void printBin(unsigned char IN, int size); 

int main (int argc, char **argv) 
{


    FILE *filePtr;
    unsigned char *fileData;
    unsigned char *dispData;
    char header[80];
    int ROWS, COLS, BYTES;
    int i,r,c;
    bool RGB = false;
    int SIZE; 

    fprintf(stdout,"\n");

    // ERROR CHECK COMMAND LINE ARGUMENTS
    if (argc != 2) 
    {
        fprintf(stdout, "Usage: lab1 [filename.ppm]\n");
        exit(0);
    }

    // OPEN FILE, VERIFY CORRECT FILE
    filePtr = fopen(argv[1], "rb");
    
    if(filePtr == NULL)
    {
        fprintf(stdout, "Unable to open file \"%s\" for reading!\n", argv[1]);
    }   
    
    i = fscanf(filePtr, "%s %d %d %d ", header, &COLS, &ROWS, &BYTES);
    if (i != 4 || (strcmp(header,"P5") != 0 && strcmp(header,"P6") != 0) || BYTES != 255)
    {
        fprintf(stdout, "Incorrect file type! Please use an 8-bit PPM image!");
        exit(0);
    }

    // CHECK IMAGE TYPE
    if(strcmp(header,"P5") == 0)
    {
        fprintf(stdout, "%s is a greyscale image!\n", argv[1]);
    } else {
        fprintf(stdout, "%s is an RGB image!\n", argv[1]);
        RGB = true;
    }
   
    // SIZE variable changed if RGB, allows me to use same alloc/read
    if(RGB) { SIZE = ROWS*COLS*3; } else { SIZE = ROWS*COLS; }
   
    // DYNAMIC ALLOCATION FOR IMAGE
    fileData = (unsigned char*)calloc(SIZE, sizeof(unsigned char));
    if(fileData == NULL)
    {
        fprintf(stdout, "Unable to allocate %d x %d memory!\n", ROWS, COLS);
        exit(0);
    }
    
    // READ IN IMAGE DATA
    fread(fileData, 1, SIZE, filePtr);
    fclose(filePtr);    // Done with file, close

    //HELPER PROGRAM: CREATING NEW IMAGE, using diff to ensure it was read correctly
    /*
    FILE *newFile = fopen("diffFile.ppm", "wb");

    fprintf(newFile,"P5 %d %d 255 \n", COLS, ROWS);
    fwrite(fileData, 1, SIZE, newFile);
    fclose(newFile);
    */


   // start data manipulation for RGB vs. Greyscale (BITSHIFTING) 
    unsigned char tmp;

    if(!RGB)    // convert 8bit greyscale to colormap TODO change to colormap
    {
       // dispData = (unsigned char*)calloc(ROWS*COLS, sizeof(unsigned char));
        dispData = fileData;

    } else {    // convert 24bit RGB to 16bit RGB TODO change blue & red for windows
        // switched blue & red, changed bit shift to allow for rightmost X bit (0 default)
        dispData = (unsigned char*)calloc(ROWS*COLS*2, sizeof(unsigned char));
        for(i = 0; i < COLS*ROWS; i++)
        {
            tmp = fileData[i*3+2] & 0xF8;   //first 5 bits of BLUE;
            dispData[i*2+1] |= tmp;

            tmp = fileData[i*3+1] & 0xF8; //first 5 bits of GREEN;
            dispData[i*2+1] |= tmp >> 5;
            dispData[i*2] |= tmp << 4;

            tmp = fileData[i*3] & 0xF8; // first 5 bits of RED
            dispData[i*2] |= tmp >> 2;
        }

    }

    // START WINDOWS STUFF
    
    

    // FREE DYNAMIC MEMORY
    free(fileData);
    if(RGB) { free(dispData); } // dispData only allocated for RGB, don't want to free same memory twice.
    fprintf(stdout,"\n");

    return 0;
}

void printBin(unsigned char IN, int size) 
{
    int i;

    for(i = size-1; i >=0; i--) {
        printf("%c", (IN & (1<<i)) ? '1' : '0');
    }
}



