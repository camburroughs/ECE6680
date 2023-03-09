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
    bool compress = false;
    bool run = true;
    bool RLE = true;
    bool single = true;
    //variables for compression
    unsigned char runCount;
    unsigned char symA, symB;
    unsigned char buffer[4];
    unsigned char marker = 0x00;
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
    char *ext_loc = strchr(argv[2],'.');
    strcpy(newName, compress ? "c" : "d");
    strcat(newName,ext_loc);
    fpt_out = fopen(newName,"wb");

    // START COMPRESSION/DECOMPRESSION
    runCount = 0;
    if(compress) // COMPRESS STUFF
    {
        runCount++;
        fread(&symA, sizeof(unsigned char), 1, fpt);

        while(run)
        {
            if((result = fread(&symB, sizeof(unsigned char), 1, fpt)) <= 0)
            {
                // REACHES END OF FILE
                if(RLE)
                {
                    fwrite(&runCount, sizeof(unsigned char), 1, fpt_out);
                }
                fwrite(&symB, sizeof(unsigned char), 1, fpt_out);
                run = 0;
            } else {
                if(symA == symB) 
                {
                    single = false;
                    if(RLE) 
                    {
                        if(runCount < 255)
                        {
                            runCount++;
                            continue;
                        }
                    } else {
                        fwrite(&marker, sizeof(unsigned char),1,fpt_out);
                        RLE = true;
                        runCount = 2;
                        continue;
                    }
                } else {
                    if(RLE) 
                    {
                        if(single) 
                        {
                            fwrite(&marker, sizeof(unsigned char),1,fpt_out);
                            RLE = false;
                        } else {
                            single = true;
                        }
                    } 
                }

                // WRITE COMPRESSION
                if(RLE) {fwrite(&runCount, sizeof(unsigned char), 1, fpt_out);}
                fwrite(&symA, sizeof(unsigned char), 1, fpt_out);
                symA = symB;
                runCount = 1;    
            } 

        }


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
        fread(&symA, sizeof(unsigned char),1,fpt);
        while(run)
        {   
            // GET RUNCOUNT
            if((result = fread(&symB, sizeof(unsigned char), 1, fpt)) <= 0)
            {
                //might need to add more stuff
                run = 0;
            } else {
                // PRINT SYMBOL
                if (symA == 0) {
                    if(RLE) 
                    {
                        RLE = false;
                        symA = symB;
                        continue;
                    }
                    RLE = true;
                    symA = symB;
                    continue;
                } else {
                    if(RLE)
                    {
                        runCount = symA;
                        for(int i = 0; i < runCount; i++) 
                        {
                            fwrite(&symB, sizeof(unsigned char), 1, fpt_out);
                        }
                        fread(&symA, sizeof(unsigned char),1,fpt);
                    } else {
                        fwrite(&symA, sizeof(unsigned char),1,fpt_out);
                        symA = symB;
                    }
                }
            }
        }
    }
    fclose(fpt);
    fclose(fpt_out);

    return 0;
}        
