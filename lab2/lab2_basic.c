/*  Cameron Burroughs | C17134631 | burrou5@clemson.edu
 *  ECE 6680 | lab2 - RLE Compression | Dr. Adam Hoover
 *  lab2.c 
 *  Description: program to compress/decompress file
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>


void printBin(unsigned char IN, int size); 

int main (int argc, char **argv) 
{
    FILE *fpt, *fpt_out;
    char newName[30];
    char ext[5];
    bool compress = false;
    bool run = 1;

    //variables for compression
    unsigned char runCount;
    unsigned char symA, symB;
    unsigned char buffer[10];
    int result = 0;

    // ERROR CHECK COMMAND LINE ARGUMENTS
    if (argc != 3 || (strcmp(argv[1],"c") != 0 && strcmp(argv[1],"d") != 0)) 
    {
        fprintf(stdout, "Usage: ./lab2 [c/d] [filename]\n");
        exit(0);
    }

    if (strcmp(argv[1],"c") == 0) { compress = true; }

    // OPEN FILE, VERIFY CORRECT FILE
    fpt = fopen(argv[2], "rb");
    
    if(fpt == NULL)
    {
        fprintf(stdout, "Unable to open file \"%s\" for reading!\n", argv[2]);
        exit(1); 
    }

    // CREATE NEW FILENAME
    // will cause seg fault if not decomp & _RLE or comp & ext
    char *ext_loc = strchr(argv[2],'.');
    strcpy(ext,ext_loc);
    if (compress) 
    {
        strcpy(newName,"c");
        strcat(newName,ext);
    } else {
        strcpy(newName,"d");
        strcat(newName,ext);
    }


    fpt_out = fopen(newName,"wb");

    // START COMPRESSION/DECOMPRESSION
    runCount = 0;
    if(compress) // COMPRESS STUFF
    {
        //GET SYM A
        fread(&symA, sizeof(unsigned char), 1, fpt);
        runCount++;
        while(run)
        {
            // GET SYM B
            if((result = fread(&symB, sizeof(unsigned char), 1, fpt)) <= 0)
            {
                // REACHES END OF FILE
                fwrite(&runCount, sizeof(unsigned char), 1, fpt_out);
                fwrite(&symB, sizeof(unsigned char), 1, fpt_out);
                run = 0;
            } else {
                if(symA == symB) 
                {
                    if(runCount < 255) {
                        runCount++;
                        continue;
                    }
                }
                // WRITE COMPRESSION
                fwrite(&runCount, sizeof(unsigned char), 1, fpt_out);
                fwrite(&symA, sizeof(unsigned char), 1, fpt_out);
                symA = symB;
                runCount = 1;    
            } 
        }
    } else {    // DECOMPRESS
        while(run)
        {   
            // GET RUNCOUNT
            if((result = fread(&runCount, sizeof(unsigned char), 1, fpt)) <= 0)
            {
                run = 0;
            } else {
                // PRINT SYMBOL
                fread(&symA, sizeof(unsigned char),1, fpt);
                for(int i = 0; i < runCount; i++) 
                {
                    fwrite(&symA, sizeof(unsigned char), 1, fpt_out);
                }
            }
        }
    }
    fclose(fpt);
    fclose(fpt_out);

    return 0;
}        
