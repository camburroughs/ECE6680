/*  Cameron Burroughs | C17134631 | burrou5@clemson.edu
 *  ECE 6680 | lab4 - Huffman Codec | Dr. Adam Hoover
 *  lab4.c 
 *  Description: program to compress/decompress file
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#define MAX_LINE_LENGTH 100
#define FLOAT_MIN -99999
#define FLOAT_MAX 99999
#define PI 3.14159265359
#define ROWS 256
#define COLS 256

typedef struct face_t {
    int indices;
    int V0, V1, V2;
} face_t;

void rotate (float objVector[3], float rotX[3][3], float rotY[3][3], float rotZ[3][3], float result[3]);
void printVector( float vector[3] );
float convertToRadians(float angle);
void posZeros(float matrix[3][3]);
void crossProd(float V1[3], float V2[3], float CP[3]);
float dotProd(float V1[3], float V2[3]);
float square(float);
void subVectors(float V1[3], float V2[3], float result[3]);

int main (int argc, char **argv) 
{
    FILE *fpt, *fpt_out;
    char newName[30];
    char ext[5];
    char cur_line[MAX_LINE_LENGTH], buffer[15];
    float angX, angY, angZ;
    bool header = true;
    int vertices, faces, num;
    int i;

    // ERROR CHECK COMMAND LINE ARGUMENTS
    if (argc != 5) 
    {
        fprintf(stdout, "Usage: ./lab5 [filename.ply] [angle X] [angle Y] [angle Z]\n");
        exit(0);
    }

    // GET FILE EXTENSION & CREATE NEW FILE NAME
    strcpy(newName, argv[1]);
    char *ext_loc = strchr(newName,'.');
    if(ext_loc != NULL) {
        strcpy(ext,ext_loc);
    }
    if(ext_loc == NULL || strcmp(ext, ".ply") != 0) {
        fprintf(stdout, "Incorrect file type! Please use a .ply file.\n");
        exit(0);
    }
    strcpy(ext_loc, ".ppm");
    
    sscanf(argv[2], "%f", &angX);
    sscanf(argv[3], "%f", &angY);
    sscanf(argv[4], "%f", &angZ);


    // OPEN FILE, VERIFY CORRECT FILE
    fpt = fopen(argv[1], "rb");
    
    if(fpt == NULL)
    {
        fprintf(stdout, "Unable to open file \"%s\" for reading!\n", argv[1]);
        exit(1); 
    }

    fpt_out = fopen(newName,"wb");

    /************************** PASE PLY FILE HEADER **************************/
    int lines = 0;
    vertices = faces = 0;
    while(header) 
    {
        fgets(cur_line, MAX_LINE_LENGTH, fpt);
        lines++;
        if(strncmp(cur_line, "element", 7) == 0) {
            sscanf(cur_line, "%*s %s %d", buffer, &num);
            if(strcmp(buffer, "vertex") == 0) {
                vertices = num;
            } else if(strcmp(buffer,"face") == 0) {
                faces = num;
            } else {
                printf("error occurred, element with identifier \"%s\" found\n", buffer);
                exit(0);
            }
        } else if((lines == 9) || (strcmp(cur_line, "end_header\n")) == 0) {
            header = false;
        }
    }
    
    /************************** READ VERTICES AND FACES **************************/
    float **vertex = (float**)calloc(vertices, sizeof(float*));
    for(i = 0; i < vertices; i++) {
        vertex[i] = (float*)calloc(3, sizeof(float));
    }
    face_t *face = (face_t*)calloc(faces, sizeof(face_t));
    
    // READ VERTICES
    i = 0;
    while(i < vertices && fgets(cur_line, MAX_LINE_LENGTH, fpt) != NULL)
    {
        sscanf(cur_line, "%f %f %f", &vertex[i][0], &vertex[i][1], &vertex[i][2]);
        i++;
    }
    
    // READ FACES
    i = 0;
    while(i < faces && fgets(cur_line, MAX_LINE_LENGTH, fpt) != NULL)
    {
        sscanf(cur_line,"%d %d %d %d", &face[i].indices, &face[i].V0, &face[i].V1, &face[i].V2);
        i++;
    }
    
    /************************** CALCULATE BOUNDING BOX **************************/
    float max[3] = {FLOAT_MIN, FLOAT_MIN, FLOAT_MIN};
    float min[3] = {FLOAT_MAX, FLOAT_MAX, FLOAT_MAX};
    float center[3] = {0, 0, 0};

    for(i = 0; i < vertices; i++) {
        // GET MAX VERTEXES
        if(max[0] < vertex[i][0]) max[0] = vertex[i][0];
        if(max[1] < vertex[i][1]) max[1] = vertex[i][1];
        if(max[2] < vertex[i][2]) max[2] = vertex[i][2];
        // GET MIN VERTEXES
        if(min[0] > vertex[i][0]) min[0] = vertex[i][0];
        if(min[1] > vertex[i][1]) min[1] = vertex[i][1];
        if(min[2] > vertex[i][2]) min[2] = vertex[i][2];
        
    }

    // DIVIDE SUM TO GET CENTER
    center[0] = (max[0] + min[0])/2;
    center[1] = (max[1] + min[1])/2;
    center[2] = (max[2] + min[2])/2;

    // FIND EXTENT OF BOUNDING BOX
    float E;
    E = max[0] - min[0];
    if(max[1] - min[1] > E) E = max[1] - min[1];
    if(max[2] - min[2] > E) E = max[2] - min[2];
    
    printf("Bounding Box:\n");
    printf("x-> %f ... %f\n", min[0], max[0]);
    printf("y-> %f ... %f\n", min[1], max[1]);
    printf("z-> %f ... %f\n", min[2], max[2]);

    printf("Sphere: %0.1f, %0.1f, %0.1f\tDiameter: %0.1f\n",center[0], center[1], center[2], E );

    /********************** CAMERA POSITION & ORIENTATION ***********************/
    float camera[3] = {1,0,0};
    float up[3] = {0,0,1};

    float radX = convertToRadians(angX);
    float radY = convertToRadians(angY);
    float radZ = convertToRadians(angZ);

    float rot_x[3][3] = {{1,0,0}, {0,cos(radX),-1*sin(radX)}, {0,sin(radX),cos(radX)}};
    float rot_y[3][3] = {{cos(radY),0,sin(radY)}, {0,1,0}, {-1*sin(radY),0,cos(radY)}};
    float rot_z[3][3] = {{cos(radZ),-1*sin(radZ),0}, {sin(radZ),cos(radZ),0}, {0,0,1}};

    posZeros(rot_x);
    posZeros(rot_y);
    posZeros(rot_z);

    float camRotated[3] = {0,0,0};
    float upRotated[3] = {0,0,0};
    
    // ROTATE CAMERA
    rotate(camera,rot_x,rot_y,rot_z,camRotated);
    camera[0] = camRotated[0];
    camera[1] = camRotated[1];
    camera[2] = camRotated[2];
    printf("Camera after rotations: ");
    printVector(camera);
    
    // ROTATE UP
    rotate(up,rot_x,rot_y,rot_z,upRotated);
    up[0] = upRotated[0];
    up[1] = upRotated[1];
    up[2] = upRotated[2];
    printf("Up after rotations: ");
    printVector(up);
    
    // MOVE AND SCALE CAMERA
    for(i = 0; i < 3; i++) {
        camera[i] = 1.5*E*camera[i] + center[i];
    }
    printf("Camera after translation/scaling: %0.2f %0.2f %0.2f\n", camera[0], camera[1], camera[2]);
 
    /********************** 3D BOUNDING COORDINATES ***********************/
    float tmp[3], left[3], right[3], top[3], bot[3], toplft[3] = {0,0,0};

    for(i = 0; i < 3; i ++) {
        tmp[i] = center[i]-camera[i];
    }

    crossProd(up, tmp, left);

    float a = sqrt(square(left[0]) + square(left[1]) + square(left[2]));

    for(i = 0; i < 3; i ++) {
        left[i] = (E/(2*a))*left[i] + center[i];   
    }

    crossProd(tmp, up, right);
    for(i = 0; i < 3; i ++) {
        right[i] = (E/(2*a))*right[i] + center[i];   
    }
    printf("camera left/right: %0.1f %0.1f %0.1f \t%0.1f %0.1f %0.1f\n", left[0], left[1], left[2], right[0], right[1], right[2]);

    for(i = 0; i < 3; i ++) {
        top[i] = (E/2)*up[i] + center[i];   
    }
    
    for(i = 0; i < 3; i ++) {
        bot[i] = (-1*E/2)*up[i] + center[i];   
    }
    
    for(i = 0; i < 3; i ++) {
        toplft[i] = (E/2)*up[i] + left[i];   
    }

    float rows = sqrt(square(left[0]) + square(left[1]) + square(left[2])) + sqrt(square(right[0]) + square(right[1]) + square(right[2]));

    printf("Camera top/bottom: %0.1f %0.1f %0.1f \t%0.1f %0.1f %0.1f\n", top[0], top[1], top[2], bot[0],bot[1],bot[2]);

    /********************** PIXEL PROCESSING ***********************/
    // list of image vector coordinates
    unsigned char* pixelData = (unsigned char*)calloc(ROWS*COLS, sizeof(unsigned char));
    float** image = (float**)calloc(ROWS*COLS,sizeof(float*));
    float* zBuff = (float*)calloc(ROWS*COLS, sizeof(float));
    for(i = 0; i < ROWS*COLS; i++) {
        image[i] = (float*)calloc(3, sizeof(float));
    }

    // list of plane coordinates <A, B, C, D>
    float** plane = (float**)calloc(faces,sizeof(float*));
    for(i = 0; i < faces; i++) {
        plane[i] = (float*)calloc(4, sizeof(float));
    }
    
    float r,c;
    float tmpV1[3], tmpV2[3], intersect[3];
    float CP1[3], CP2[3];
    float n, d, dot1, dot2, dot3;
    float ray[3];
    float negABC[3];

    printf("Rendering... ");
    // FOR EACH PIXEL 
    for(int p = 0; p < ROWS*COLS; p++) {
        r = p/ROWS;
        c = p%COLS;
        
        if(c == 0)
            printf("%0.0f ", r);

        zBuff[p] = 999999;
        
        for(i = 0; i < 3; i++) {
            image[p][i] = toplft[i] + ((float)c/(COLS-1))*(right[i]-left[i]) + ((float)r/(ROWS-1))*(bot[i]-top[i]);
        }

        // FOR EACH TRIANGE W COORDINATES V0, V1, AND V2
        for(int f = 0; f < faces; f++) {
            // FINDING <A,B,C,D> 
            subVectors(vertex[face[f].V1], vertex[face[f].V0], tmpV1);
            subVectors(vertex[face[f].V2], vertex[face[f].V0], tmpV2);
            crossProd(tmpV1,tmpV2,plane[f]);
            plane[f][3] = -1*dotProd(plane[f], vertex[face[f].V0]);

            // FINDING n & d
            n = (-1*dotProd(plane[f], camera)) - plane[f][3];
            subVectors(image[p],camera, tmpV1);
            d = dotProd(plane[f],tmpV1);
            // if d is near zero, skip triangle
            if(fabs(d) < 0.01) {
                continue; 
            }
            
            // RAY FROM CAMERA TO PIXEL
            subVectors(image[p], camera, ray);
//            printf("\n\tXr,Yr,Zr: ");
//            printVector(ray);
            
//            printf("\tCamera: ");
//            printVector(camera);

            for(i = 0; i < 3; i++) {
                intersect[i] = camera[i] + ((n/d)*ray[i]);
            }
//            printf("\tintersect: ");
//            printVector(intersect);
            

/*            printf("\n\tV0: ");
            printVector(vertex[face[f].V0]);
            printf("\tV1: ");
            printVector(vertex[face[f].V1]);
            printf("\tV2: ");
            printVector(vertex[face[f].V2]);
*/
            subVectors(vertex[face[f].V2], vertex[face[f].V0], tmpV1);  // <V2 - V0>
            subVectors(vertex[face[f].V1], vertex[face[f].V0], tmpV2);  // <V1 - V0>
            crossProd(tmpV1, tmpV2, CP1);   // <V2 - V0> x <V1-V0>
            subVectors(intersect, vertex[face[f].V0], tmpV1);   // <intersect - V0>
            crossProd(tmpV1, tmpV2, CP2); // <intersect - V0> x <V1 - V0>
            dot1 = dotProd(CP1,CP2);

            subVectors(vertex[face[f].V0], vertex[face[f].V1], tmpV1); // <V0 - V1>
            subVectors(vertex[face[f].V2], vertex[face[f].V1], tmpV2); // <V2 - V1>
            crossProd(tmpV1, tmpV2, CP1);   // <V0 - V1> x <V2 - V1> 
            subVectors(intersect, vertex[face[f].V1], tmpV1);   // <intersect - V1>
            crossProd(tmpV1, tmpV2, CP2);   // <intersect - V1> x <V2 - V1>
            dot2 = dotProd(CP1,CP2);

            subVectors(vertex[face[f].V1], vertex[face[f].V2], tmpV1);  // <V1 - V2>
            subVectors(vertex[face[f].V0], vertex[face[f].V2], tmpV2);  // <V0 - V2>
            crossProd(tmpV1, tmpV2, CP1);   // <V1 - V2> x <V0 - V2>
            subVectors(intersect, vertex[face[f].V2], tmpV1);   // <intersect - V2>
            crossProd(tmpV1, tmpV2, CP2);   // <intersect - V2> x <V0 - V2>
            dot3 = dotProd(CP1,CP2);
            
//            printf("\n\tdot1: %0.3f\n\tdot2: %0.3f\n\tdot3: %0.3f\n\n", dot1, dot2, dot3);

            if(dot1 < 0 || dot2 < 0 || dot3 < 0) {
                continue; // skip triangle
            } 

            //printf("n/d: %0.4f\n", n/d);
            if( (n/d) > zBuff[p] )
                continue;
            zBuff[p] = (n/d);
            pixelData[p] = 155 + (f%100);
            //printf("f: %d\n", f);
        }
        //printf("zbuff: %0.3f\n", zBuff[p]);
        //printf("pixel value: %0.2f\n", (float)pixelData[p]);
    }

    printf("\n");

    char *PPMHeader = "P5 256 256 255\n";
    fwrite(PPMHeader, sizeof(unsigned char), strlen(PPMHeader), fpt_out);
    fwrite(pixelData, ROWS*COLS, sizeof(unsigned char), fpt_out);

    // CLOSE FILES
    fclose(fpt);
    fclose(fpt_out);

    // FREE MEMORY
    for(i = 0; i < vertices; i ++)
        free(vertex[i]);
    free(vertex);
    for(i = 0; i < ROWS*COLS; i++)
        free(image[i]);
    free(image);
    for(i = 0; i <faces; i++)
        free(plane[i]);
    free(plane);
    free(pixelData);
    
    return 0;
}

void subVectors(float V1[3], float V2[3], float result[3]){
    for(int i = 0; i < 3; i++) {
        result[i] = V1[i] - V2[i];
    }
}

float square(float x) {
    return x*x;
}

float dotProd(float V1[3], float V2[3])
{
    float sum = 0;
    for(int i = 0; i < 3; i++) {
        sum += V1[i]*V2[i];
    }
    return sum;
}

void crossProd(float V1[3], float V2[3], float CP[3])
{
    CP[0] = V1[1] * V2[2] - V1[2] * V2[1];
    CP[1] = V1[2] * V2[0] - V1[0] * V2[2];
    CP[2] = V1[0] * V2[1] - V1[1] * V2[0];
}

void posZeros(float matrix[3][3]) 
{
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            if(matrix[i][j] > 0 && matrix[i][j] < 0.0001)
                matrix[i][j] = 0;
            if(matrix[i][j] < 0 && (0 - matrix[i][j]) < 0.0001 )
                matrix[i][j] *= -1;
            if(matrix[i][j] == 0)
                matrix[i][j] = 0;
        }
    }
}

//TODO remove -1
float convertToRadians(float angle)
{
    return -1 * angle * (float)(PI/180); 
}

void printVector( float vector[3] ) 
{
    for(int i = 0; i <3; i++) {
        printf("%0.4f ", vector[i]);
    }
    printf("\n");
}



void rotate (float objVector[3], float rotX[3][3], float rotY[3][3], float rotZ[3][3], float result[3])
{
    int row,col;
    float tmp1[3] = {0,0,0};
    float tmp2[3] = {0,0,0};
    // ROTATE X
    for(col = 0; col < 3; col++) {
        for(row = 0; row < 3; row++) {
            tmp1[col] += objVector[row] * rotX[row][col];
        }    
    }
    // ROTATE Y
    for(col = 0; col < 3; col++) {
        for(row = 0; row < 3; row++) {
            tmp2[col] += tmp1[row] * rotY[row][col];
        }    
    }
    // ROTATE Z
    for(col = 0; col < 3; col++) {
        for(row = 0; row < 3; row++) {
            result[col] += tmp2[row] * rotZ[row][col];
        }    
    }

}
