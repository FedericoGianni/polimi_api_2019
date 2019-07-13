#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#define ADDENT "addent"
#define DELENT "delent"
#define ADDREL "addrel"
#define DELREL "delrel"
#define REPORT "report"
#define END "end"

#define DEF_STRING_L 1024

char *readStdIn();


void main() {

    bool end = false;
    char *input;

    while(!end){

        //read input from stdin, line by line allocating a dynamic array for each line
        input = readStdIn();
        printf("\n[DEBUG] Letta la stringa: %s", input);

        if(strncmp(input, ADDENT, 6) == 0){
            printf("[DEBUG] read addent\n");

        } else if(strncmp(input, DELENT, 6) == 0){
            printf("[DEBUG] read deleent\n");

        } else if(strncmp(input, ADDREL, 6) == 0){
            printf("[DEBUG] read add rel\n");

        } else if(strncmp(input, DELREL, 6) == 0){
            printf("[DEBUG] read del rel\n");

        } else if(strncmp(input, REPORT, 6) == 0){
            printf("[DEBUG] read repo\n");

        } else if(strncmp(input, END, 3) == 0){
            printf("[DEBUG] end!");
            end = true;

        } else {
            perror("[DEBUG] invalid command read!");
            break;
        }

        //de-allocate heap memory used to store string input, since in the next execution the pointer will point to
        //the new string and it will be impossible to eliminate old string from heap
        free(input);

    }
}

char* readStdIn() {

    char* input = NULL;
    char tempbuf[DEF_STRING_L];
    size_t inputlength = 0, templength = 0;

    do {

        fgets(tempbuf, DEF_STRING_L, stdin);
        templength = strlen(tempbuf);
        inputlength += templength;
        input = realloc(input, inputlength+1);
        strcat(input, tempbuf);

    } while (templength==DEF_STRING_L-1 && tempbuf[DEF_STRING_L-2]!='\n');

    return input;
}