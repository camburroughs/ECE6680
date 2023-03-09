/*  Cameron Burroughs | C17134631 | burrou5@clemson.edu
 *  ECE 6680 | lab8 - Huffman Codec | Dr. Adam Hoover
 *  lab8.c 
 *  Description: RMA Scheduling
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>



int main(int argc, char** argv) 
{   
    // feature table
    char feature[8][100] = {
        "Compute attitude data",
        "Compute velocity data",
        "Compose attitude message",
        "Display data",
        "Compose navigation message",
        "Run-time Built-In Test",
        "Compute position data",
        "Compose test message"
    };


    float period[8] = {10.56, 40.96, 61.44, 100, 165, 285, 350, 700};
    float runtime[8] = {1.30, 4.70, 9.00, 23.00, 38.30, 10.00, 3.00,2.00};
    float blocking[8] = {3.3, 9.3, 9.3, 5.2, 5.2, 5.2, 0.0, 0.0};

    int i,k,l,j;
    float sum;

    for(i = 0; i < 8; i++) {
        for(k = 0; k < i+1; k++) {
            for(l = 0; l < floor(period[i]/period[k]); l++) {
                sum = 0; 
                for(j = 0; j < i; j++) {
                    sum += (runtime[j] + 0.153) * ceil(((l+1)*period[k])/period[j]);
                }

                sum += runtime[i] + 0.153 + blocking[i];

                if(sum < period[k]*(l+1)) {
                    printf("%s is schedulable w/ k = %d and l = %d\n", feature[i], k+1, l+1);
                    break;
                }

            }
            if(l < floor(period[i]/period[k])) {
                break;
            }
        }
        if(k == i+1){
            printf("%s not scheduable\n", feature[i]);
        }
    }
    return 0;


}

