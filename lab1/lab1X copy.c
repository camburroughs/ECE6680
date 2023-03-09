/*  Cameron Burroughs | C17134631 | burrou5@clemson.edu
 *  ECE 6680 | lab1 - Image Diaplay | Dr. Adam Hoover
 *  lab1.c 
 *  Description: program to load in ppm pixel data
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <unistd.h>


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

    Display *Monitor;
    Window ImageWindow;
    GC ImageGC;
    XWindowAttributes Atts;
    XImage *Picture;
    

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

    if(!RGB)    // convert 8bit greyscale to 16bit RGB 
    {
        dispData = (unsigned char*)calloc(ROWS*COLS*2, sizeof(unsigned char));
        for(i = 0; i < ROWS*COLS; i++)
        {
            tmp = fileData[i] & 0xF8;   // first 5 bits
            dispData[i*2+1] |= tmp;         // set red
            dispData[i*2] |= tmp >> 3;  // set blue

            tmp = fileData[i] & 0xFC;   // first 6 bits
            dispData[i*2+1] |= tmp >> 5;      // set green
            dispData[i*2] |= tmp << 3;
            
          /*  printf("fileData[%d]: ", i);
            printBin(fileData[i], 8);
            printf("\n");
            printf("dispData: ");
            printBin(dispData[i*2], 8);
            printBin(dispData[i*2+1], 8);
            printf("\n"); */

        }
    } else {    // convert 24bit RGB to 16bit RGB
        dispData = (unsigned char*)calloc(ROWS*COLS*2, sizeof(unsigned char));
        for(i = 0; i < COLS*ROWS; i++)
        {
            tmp = fileData[i*3] & 0xF8;   //first 5 bits of RED;
            dispData[i*2+1] |= tmp;

            tmp = fileData[i*3+1] & 0xFC; //first 6 bits of GREEN;
            dispData[i*2+1] |= tmp >> 5;
            dispData[i*2] |= tmp << 3;

            tmp = fileData[i*3+2] & 0xF8; // first 5 bits of BLUE
            dispData[i*2] |= tmp >> 3;
        }

    }

    // START WINDOWS STUFF
    Monitor = XOpenDisplay(NULL);
    if (Monitor == NULL) 
    {
        printf("unable to open graphics display!\n");
        exit(0);
    }

    ImageWindow=XCreateSimpleWindow(Monitor,RootWindow(Monitor,0),
    	50,10,		// x,y on screen 
    	COLS,ROWS,	// width, height 
    	2, 		// border width 
    	BlackPixel(Monitor,0),
    	WhitePixel(Monitor,0));
        
    ImageGC=XCreateGC(Monitor,ImageWindow,0,NULL);

    XMapWindow(Monitor,ImageWindow);
    XFlush(Monitor);

    while(1)
    {
        XGetWindowAttributes(Monitor,ImageWindow,&Atts);
        if (Atts.map_state == IsViewable) //2
        break;
    }

    Picture=XCreateImage(Monitor,DefaultVisual(Monitor,0),
		    DefaultDepth(Monitor,0),
		    ZPixmap,	// format 
		    0,		// offset 
		    (char*) dispData,// the data 
		    COLS,ROWS,	// size of the image data 
		    16,		// pixel quantum (8, 16 or 32) 
		    0);		// bytes per line (0 causes compute)


    XPutImage(Monitor,ImageWindow,ImageGC,Picture,
		    0,0,0,0,	// src x,y and dest x,y offsets 
		    COLS,ROWS);	// size of the image data 
    XFlush(Monitor);
    sleep(3);
    XCloseDisplay(Monitor);
    

    // FREE DYNAMIC MEMORY
    free(fileData);

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



