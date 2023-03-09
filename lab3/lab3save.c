/*  Cameron Burroughs | C17134631 | burrou5@clemson.edu
 *  ECE 6680 | lab3 - LZW Compression | Dr. Adam Hoover
 *  lab3.c 
 *  Description: program to compress/decompress file
 */

/* TODO NOTES 2/21/2022
 *  - issue with compression: repeats last,  commented out lat fwrite to counteract
 *  - compression seems correct with simple text files
 *  - decompression issues: pattern of 5 repeats shows up as 4 repeats;
 *  - may be a variable issue: reconstructs dictionary correctly
 *  - fixed decomp issue by fixing a typo, adding p_len[P]+1
 *  - compressing adding xtra byte, decompression excluding new line
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#define CODE_MAX 65536
#define LEN_MAX 100

unsigned char **dict;
int *p_len;
unsigned short *codes;

void printDict(int start, int end);
int searchDict(unsigned char *pattern, int cur_len, int d_size);

int main (int argc, char **argv) 
{
    FILE *fpt, *fpt_out;
    char newName[30];
    char ext[5];
    bool compress = false;
    bool run = 1;

    int i, result, cur_len, d_size, index;
    unsigned short P_i; 

    // ERROR CHECK COMMAND LINE ARGUMENTS
    if (argc != 3 || (strcmp(argv[1],"c") != 0 && strcmp(argv[1],"d") != 0)) 
    {
        fprintf(stdout, "Usage: ./lab3 [c/d] [filename]\n");
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

    //DYNAMICALLY ALLOCATE DICTIONARY
    dict = (unsigned char**)calloc(sizeof(unsigned char*),CODE_MAX);
    for( i = 0; i < 256; i++ ) {
        dict[i] = (unsigned char*)calloc(sizeof(unsigned char), 1);
    } 
    p_len = (int *)calloc(sizeof(int), CODE_MAX);
    codes = (unsigned short*)calloc(sizeof(unsigned short),CODE_MAX);

    // INITIALIZE DICTIONARY WITH ROOTS
    d_size = 0;
    for(i = 0; i <256; i++) {
        *dict[i] = i;
        p_len[i] = 1;
        d_size++;
    }
    // FILL CODE VALUES
    for(i = 0; i <CODE_MAX; i++){
        codes[i] = 0xFFFF & i;
    }  
    // START COMPRESSION/DECOMPRESSION
    if(compress) // COMPRESS STUFF
    {
        /* PSUEDO-CODE
         *   initialize dictionary w roots
         *   P = (empty)
         *   while(not EOF)
         *  > read c (current)
         *  > if (P+C) in dictionary -> P = P+C
         *      > else -> output code for P
         *      > add P+C to dictionary
         *      > let P=C
         *  > more data? continue while
         *  EOF: output code for P, DONE
         */
        //variables for compression
        unsigned char *P, C, *P_C;


        cur_len = 0;

        P = (unsigned char*)calloc(sizeof(unsigned char), 1);
        P_C = (unsigned char*)calloc(sizeof(unsigned char), 1);

        while((result = fread(&C, sizeof(unsigned char), 1, fpt)) > 0)
        {
            P_C = (unsigned char *)realloc(P_C, sizeof(unsigned char) * (cur_len+1));

            // make code to create P+C
            for(i = 0; i < cur_len; i++) {
                P_C[i] = P[i];
            } 
            P_C[cur_len] = C;
            

            //SEARCH DICTIONARY
            
            index = searchDict(P_C, cur_len + 1, d_size);
            if(index != -1) {
            // make code to copy P+C into P
                cur_len++;
                P = (unsigned char *)realloc(P, sizeof(unsigned char) * cur_len);
                for( i = 0; i < cur_len; i++ ) {
                    P[i] = P_C[i];
                }
            } else {
                P_i = searchDict(P, cur_len,d_size);
                fwrite(&codes[P_i], sizeof(unsigned short), 1,  fpt_out);
                // add P+C to dictionary
                dict[d_size] = (unsigned char*)calloc(sizeof(unsigned char), (cur_len+1));
                for(i = 0; i <= cur_len; i++) {
                    dict[d_size][i] = P_C[i];
                }
                p_len[d_size] = cur_len+1;
                d_size++;
                cur_len = 1;
                P = realloc(P, sizeof(unsigned char) * cur_len);
                *P = C;
            }
        }
        fwrite(&codes[P_i], sizeof(unsigned short), 1,  fpt_out);
        free(P);
        free(P_C);

        //printDict(255, 270);

    } else {    // DECOMPRESS
        unsigned short P, C;
        unsigned char *X, *X_Y, *X_Z, Y, Z;
        
        cur_len = 0;

        X = (unsigned char*)calloc(sizeof(unsigned char), 1);
        X_Y = (unsigned char*)calloc(sizeof(unsigned char), 1);
        X_Z = (unsigned char*)calloc(sizeof(unsigned char), 1);
        
        fread(&C,sizeof(unsigned short), 1, fpt);
        fwrite(dict[C], sizeof(unsigned char), cur_len+1, fpt_out);
        
        while(run)
        {
            P = C;
            if((result = fread(&C, sizeof(unsigned short), 1, fpt)) <= 0)
                break;
            if(dict[C] != NULL) {
                // output C
                fwrite(dict[C], sizeof(unsigned char), p_len[C], fpt_out);
                // let X = pattern for P
                X = realloc(X, sizeof(unsigned char)*p_len[P]);
                for(i = 0; i < p_len[P]; i++) {
                    X[i] = dict[P][i];
                }
                //let Y = 1st char of pattern for C
                Y = dict[C][0];
                //add X+Y to dictionary
                dict[d_size] = (unsigned char*)calloc(sizeof(unsigned char), p_len[P]+1);
                X_Y = realloc(X_Y, sizeof(unsigned char)*p_len[P]+1);
                for(i = 0; i < p_len[P]; i++){
                    X_Y[i] = X[i];
                }
                X_Y[p_len[P]] = Y;
                for(i = 0; i < p_len[P]+1; i++) {
                    dict[d_size][i] = X_Y[i];
                }
                p_len[d_size] = p_len[P]+1;
                d_size++;

            } else {
                // let X = pattern for P
                X = realloc(X, sizeof(unsigned char)*p_len[P]);
                for(i = 0; i < p_len[P]; i++) {
                    X[i] = dict[P][i];
                }
                // let Z = 1st char of pattern for P
                Z = dict[P][0];
                // output X_Z
                X_Z = realloc(X_Z, sizeof(unsigned char)*p_len[P]+1);

                for(i = 0; i < p_len[P]; i++){
                    X_Z[i] = X[i];
                }
                X_Z[p_len[P]] = Z;
                fwrite(X_Z, sizeof(unsigned char), p_len[P]+1, fpt_out);
                // add X+Z to dictionary
                dict[d_size] = (unsigned char*)calloc(sizeof(unsigned char), p_len[P]+1);
                for(i = 0; i < p_len[P]+1; i++) {
                    dict[d_size][i] = X_Z[i];
                }
                p_len[d_size] = p_len[P]+1;
                d_size++;
                
                
            }

        }
       // printDict(255, 280);
        free(X);
        free(X_Y);
        free(X_Z);
    }

    //CLOSE FILES & RECURSIVELY FREE DICTIONARY
    fclose(fpt);
    fclose(fpt_out);

    for(i = 0; i < d_size; i++){
        free(dict[i]);
    }
    free(dict);
    free(codes);

    return 0;
}

int searchDict(unsigned char *pattern, int cur_len, int d_size)
{
    int i, index = -1;

    for( i = 0; i < d_size; i++ ) {
        if(cur_len == p_len[i]) {
            if(memcmp(pattern, dict[i], cur_len) == 0) {
                index = i;
                break;
            }
        }
    }
    return index;
}

void printDict(int start, int end) 
{
    int i;

    printf("%-10s|%-10s\n", "code", "pattern");

    for(i = start; i < end; i++) {
        printf("%-10d|%-10s\n", codes[i], dict[i]);
    }
}
