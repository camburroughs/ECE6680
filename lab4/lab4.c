/*  Cameron Burroughs | C17134631 | burrou5@clemson.edu
 *  ECE 6680 | lab4 - Huffman Codec | Dr. Adam Hoover
 *  lab4.c 
 *  Description: program to compress/decompress file
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#define MAX 256
#define parent(x) (x-1)/2
#define l_child(x) x*2+1
#define r_child(x) x*2+2

typedef struct node_t {
    unsigned char ch;
    unsigned int freq;
    struct node_t* left;
    struct node_t* right;
} node;

typedef struct tree_t {
    int size;
    node** nodes; 
} tree_t;

typedef struct code_t {
    char info[MAX];
    unsigned int bits;
} code;

unsigned int *c_freq;
code *c_codes;
node **PQ;
node *root;

void printTree(node *root, int space);
node* removeMinNode(tree_t *T);
void insertNode(tree_t *T, node *N);
void getCodes(node* N, char cd[], int b);

node* makeNode(unsigned char c, unsigned int f, node* l, node* r );
bool isLeaf(node* N);
void printQ(int n);
void swapNodes(node **a, node **b);
void heapify(tree_t* T, int index);
void heapBuild(tree_t* T);

int main (int argc, char **argv) 
{
    FILE *fpt, *fpt_out;
    char newName[30];
    char ext[5];
    bool compress = false;
    bool run = 1;

    int i, result, num_c = 0;
    unsigned char C;

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

    c_freq = (unsigned int*)calloc(sizeof(unsigned int),MAX);
    PQ = (node**)calloc(sizeof(node*), MAX);
    c_codes = (code*)calloc(sizeof(code),MAX);
    
    // START COMPRESSION/DECOMPRESSION
    if(compress) // COMPRESS STUFF
    {
        // FIND FREQUENCY OF EACH CHARACTER; 
        while((result = fread(&C, sizeof(unsigned char),1,fpt)) > 0)
        {
            if(!c_freq[C]) 
            { 
                PQ[num_c] = makeNode(C,0,NULL,NULL);
                num_c++; 
            }
            c_freq[C]++;
            
        }

        PQ = (node**)realloc(PQ, sizeof(node*)*num_c);

        // sort priority queue (bubblesort)
        int j;
        node* tmp;
        for(i = 0; i < num_c-1; i++) {
            for(j = 0; j < num_c-i-1; j++) {
                if(c_freq[PQ[j]->ch] > c_freq[PQ[j+1]->ch]) {
                    tmp = PQ[j];
                    PQ[j] = PQ[j+1];
                    PQ[j+1] = tmp;
                } else if (c_freq[PQ[j]->ch] == c_freq[PQ[j+1]->ch]){
                    if(PQ[j]->ch > PQ[j+1]->ch) {
                    tmp = PQ[j];
                    PQ[j] = PQ[j+1];
                    PQ[j+1] = tmp;    
                    }
                }
            }
        }
        
        //assign frequencies
        for(i = 0; i < num_c; i++) {
            PQ[i]->freq = c_freq[PQ[i]->ch];
        }
       
        //START MAKING TREE
        tree_t *tree = (tree_t*)calloc(sizeof(tree_t),1);
        tree->size = 0;
        tree->nodes = (node**)calloc(sizeof(node*),MAX);
        for(i = 0; i < num_c; i++)
        {
            tree->nodes[i] = makeNode(PQ[i]->ch, PQ[i]->freq, NULL, NULL);
        }
        tree->size = num_c;
        heapBuild(tree);

        node *min1, *min2, *sum;

        while(tree->size > 1) {
            //get two min frequencies
            min1 = removeMinNode(tree);
            min2 = removeMinNode(tree);
            
            //create node sum of two frequencies
            sum = makeNode(0, min1->freq+min2->freq, min1, min2);
            insertNode(tree,sum);
        }
        root = removeMinNode(tree);
    //    printTree(root, 0);

        // write frequencies to output file
        for(i = 0; i < MAX; i++) {
            fwrite(&c_freq[i], sizeof(unsigned int), 1, fpt_out);
        }

        //tree is complete, begin traversing tree to write codes.
        rewind(fpt);
        int b = 0; 
        char cur_code[MAX];
        unsigned int freq;

        //create codes
        getCodes(root, cur_code, b);
       
        //printTree(root, 0);
        unsigned char buff = 0x00;
        int code_bits = 0, buff_bits = 0, i = 0;
        bool res;
        
        //STAR WRITING CODES
        while((result = fread(&C, sizeof(unsigned char),1,fpt)) > 0)
        {

            strcpy(cur_code, c_codes[C].info);
            code_bits = c_codes[C].bits;
            
            while(buff_bits < 8) {
                if(i >= code_bits) {
                    i = 0;
                    break;
                }
                buff = buff << 1;
                res = (cur_code[i] == '1') ? 1 : 0;
                buff |= res;
                buff_bits++;
                i++;               
                
                if(buff_bits >= 8) {
                    fwrite(&buff, sizeof(unsigned char), 1, fpt_out);
                    buff = 0x00;
                    buff_bits = 0;
                }
            } 
        }
        if(buff_bits != 0) {
            buff = buff << (8 - buff_bits);
            fwrite(&buff, sizeof(unsigned char),1,fpt_out);
        }

    } else {    // DECOMPRESS
        // first, get frequencies for tree;
        for(i = 0; i < MAX; i++) {
            fread(&c_freq[i], sizeof(unsigned int), 1, fpt);
        }
        // create priority queue
        int j = 0;
        for(i = 0; i < MAX; i++)
        {
            if(c_freq[i] != 0){
                PQ[num_c] = makeNode(i, c_freq[i], NULL, NULL);
                num_c++;
            }
        }

        // sort priority queue (bubblesort)        
        node* tmp;
        for(i = 0; i < num_c-1; i++) {
            for(j = 0; j < num_c-i-1; j++) {
                if(c_freq[PQ[j]->ch] > c_freq[PQ[j+1]->ch]) {
                    tmp = PQ[j];
                    PQ[j] = PQ[j+1];
                    PQ[j+1] = tmp;
                } else if (c_freq[PQ[j]->ch] == c_freq[PQ[j+1]->ch]){
                    if(PQ[j]->ch > PQ[j+1]->ch) {
                    tmp = PQ[j];
                    PQ[j] = PQ[j+1];
                    PQ[j+1] = tmp;    
                    }
                }
            }
        }

        //printQ(num_c);
        
        //START MAKING TREE
        tree_t *tree = (tree_t*)calloc(sizeof(tree_t),1);
        tree->size = 0;
        tree->nodes = (node**)calloc(sizeof(node*),MAX);
        for(i = 0; i < num_c; i++)
        {
            tree->nodes[i] = makeNode(PQ[i]->ch, PQ[i]->freq, NULL, NULL);
        }
        tree->size = num_c;
        heapBuild(tree);

        node *min1, *min2, *sum;

        while(tree->size > 1) {
            //get two min frequencies
            min1 = removeMinNode(tree);
            min2 = removeMinNode(tree);
            
            //create node sum of two frequencies
            sum = makeNode(0, min1->freq+min2->freq, min1, min2);
            insertNode(tree,sum);
        }
        root = removeMinNode(tree);

        // GET  CODES
        int b = 0; 
        char cur_code[MAX];
        getCodes(root, cur_code, b);

        unsigned char buff;
        int buff_bits = 0;
        bool bit; 
        node *N = root;
        
        while((result = fread(&buff, sizeof(unsigned char),1,fpt)) > 0)
        {
          //  printf("Byte Read: ");
            buff_bits = 0;

            while(buff_bits < 8)
            {
                if(isLeaf(N)){
                //    printf("writing char: %c\n", N->ch);
                    fwrite(&N->ch, sizeof(unsigned char), 1, fpt_out);
                    N = root;
                }
                
                if(buff & (1 << (7-buff_bits))) {
                    bit = 1;
                } else { 
                    bit = 0;
                }

                buff_bits++;
            //    printf("bit: %d\n", bit);
               
                if(bit) {
                    N = N->right;
                } else {
                    N = N->left;
                }
            }


        } 
    }

    //CLOSE FILES & RECURSIVELY FREE NODES
    fclose(fpt);
    fclose(fpt_out);
    for(i = 0; i < num_c; i++){
        free(PQ[i]);
    }
    free(PQ);
    free(c_freq);


    return 0;
}



void printTree(node *root, int space)
{
    if(root == NULL)
        return;

    space += 10;
    printTree(root->right, space);
    printf("\n");
    for (int i = 10; i < space; i++)
        printf(" ");
    if(isLeaf(root))
        printf("%d | 0x%02X\n", root->freq, root->ch);
    else 
        printf("%d\n", root->freq);

    printTree(root->left, space);
}

void getCodes(node* N, char cd[], int b)
{
    if(N->left) {
        cd[b] = '0';
        getCodes(N->left, cd, b+1);
    }

    if(N->right) {
        cd[b] = '1';
        getCodes(N->right, cd, b+1);
    }

    if(isLeaf(N)) {
        cd[b] = '\0';
        strcpy(c_codes[N->ch].info, cd);
        c_codes[N->ch].bits = b;
    }
}

node* removeMinNode(tree_t *T) 
{
    node *tmp = T->nodes[0];
    T->nodes[0] = T->nodes[T->size-1];
    T->size--;
    heapify(T, 0);

    return tmp;
}

void heapBuild(tree_t* T)
{
    int i, n = T->size;

    for(i = parent(n); i >= 0; i--)
        heapify(T, i);
}

//standard recursive heapify fn to sort heap min to max
void heapify(tree_t* T, int index)
{
    int small = index;
    int left = l_child(index);
    int right = r_child(index);

    if(left < T->size && T->nodes[left]->freq < T->nodes[small]->freq)
        small = left;

    if(right <T->size && T->nodes[right]->freq < T->nodes[small]->freq)
        small = right;

    if(index != small)
    {
        swapNodes(&T->nodes[small], &T->nodes[index]);
        heapify(T, small);
    }
}

void insertNode(tree_t* T, node* N)
{
    int i = T->size;
    T->size++;
    while(i && N->freq < T->nodes[parent(i)]->freq) {
        T->nodes[i] = T->nodes[parent(i)];
        i = parent(i);
    }
    T->nodes[i] = N;
}

void swapNodes(node **a, node **b)
{
    node *tmp;
    tmp = *a;
    *a = *b;
    *b = tmp;
}

void printQ(int n)
{
    int i;
    for( i = 0; i < n; i++ ) {
        printf("PQ[%d]: %c, freq: %d\n", i, PQ[i]->ch, PQ[i]->freq);
    }
}

bool isLeaf(node* N) 
{
    return (N->left == NULL && N->right == NULL) ? 1 : 0;
}

node* makeNode(unsigned char c, unsigned int f, node* l, node* r)
{
    node* new = (node*)calloc(sizeof(node),1);
    new->ch = c;
    new->freq = f;
    new->left = l;
    new->right = r;

    return new;
}

