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

//initial input line length
#define DEF_INPUT_L 1024
//input length increment
#define DEF_INPUT_L_INC 16

//entity hash table length
#define DEF_ENT_N 1024

//initial entity_id length
#define DEF_ENT_ID_L 10

//initial relations type array length
#define DEF_REL_T_L 12

//initial relation types id length
#define DEF_REL_T_ID_L 10

//GLOBAL DATA STRUCTURES

typedef struct entity {

    //pointer to a dynamic array which stores the id of the entity
    char *id_ent;

    struct entity *next;

} entity;

typedef struct relation{

    //pointer to the entity who is giving the relation
    entity *sender;

    //pointer to the entity who is receiving the relation
    entity *receiving;


}relation;


typedef struct relation_t {

    //pointer to a dynamic array which stores the id of the relation type
    char *id_rel;

    //pointer to a dynamic array of pointers to the entities which have the max incoming relationships for this relation type
    entity *max_inc_rels;

    //pointer to a dynamic array of relation, which stores every single relation of this type
    relation *relation_list;


} relation_t;

//GLOBAL VARIABLES
//hash table to store the entities
entity *entity_hash [DEF_ENT_N];

//FUNCTIONS DEFINITION
char *readStdIn();
char *inputString(FILE *,int);


int hash(char *);


//bool addEnt(char *, entity *, int );
bool addEnt(char *, entity*);
bool delEnt(char *str, entity *entity_arr);
void report();


void main() {

    bool end = false;

    //dynamic array to read input lines
    char *input;

    //input without "" and other shit
    char *short_name;


    //dynamic array to store the relationships
    relation_t *relation_array;

    relation_array = (relation_t*) malloc(sizeof(relation_t)*DEF_REL_T_L);

    while(!end){

        //read input from stdin, line by line allocating a dynamic array for each line
        input = inputString(stdin, DEF_INPUT_L);

        printf("\n[DEBUG] Letta la stringa: %s", input);


        if(strncmp(input, ADDENT, 6) == 0){

            short_name = (char *) malloc((strlen(input) -10)* sizeof(char));
            strncpy(short_name, input + 7 * sizeof(char), strlen(input) - 7);
            printf("\n[DEBUG] Short_name: %s", short_name);

            printf("\n[DEBUG]----------------- chiamo addEnt per aggiungere un entità-------------");
            addEnt(short_name, entity_hash[0]);

        } else if(strncmp(input, DELENT, 6) == 0){
            //TODO remove this entity also from every relation
            printf("[DEBUG] read deleent\n");


        } else if(strncmp(input, ADDREL, 6) == 0){
            printf("[DEBUG] read add rel\n");

        } else if(strncmp(input, DELREL, 6) == 0){
            printf("[DEBUG] read del rel\n");

        } else if(strncmp(input, REPORT, 6) == 0){
            printf("[DEBUG] read repo\n");
            report();

        } else if(strncmp(input, END, 3) == 0){
            printf("[DEBUG] end!");
            end = true;

        } else {
            perror("[DEBUG] invalid command read!");
            break;
        }

        //de-allocate heap memory used to store string input, since in the next execution the pointer will point to
        //the new string and it will be impossible to eliminate old string from heap

        //TODO forse non servono perchè tanto vengono sovrascritti a ogni giro e mi rallenta
        free(input);

    }
}

char *inputString(FILE* fp, int size){

    char *str;
    int ch;
    size_t len = 0;
    str = (char*) malloc(sizeof(char)*size);
    if(!str)return str;
    while(EOF!=(ch=fgetc(fp)) && ch != '\n'){
        str[len++]=ch;
        if(len==size){
            str = realloc(str, sizeof(char)*(size+=DEF_INPUT_L_INC));
            if(!str)return str;
        }
    }
    str[len++]='\0';

    return realloc(str, sizeof(char)*len);
}

//replace the relation types array passed as a pointer with a new relation_t array of incremented size
relation_t *replace_rel_t_arr(relation_t *rel_arr, int l_max, int inc) {

    relation_t *new_rel_arr;

    new_rel_arr = (relation_t*) malloc(sizeof(relation_t)*(l_max+inc));

    for (int i = 0; i < l_max; ++i) {
        new_rel_arr[i] = rel_arr[i];
    }

    free(rel_arr);

    return new_rel_arr;
}

//replace the string passed as a pointer with a new string of incremented size
char *replace_char_arr(char *str, int l_max, int inc) {

    char *new_char_arr;

    new_char_arr = (char*) malloc(sizeof(char)*(l_max+inc));

    for (int i = 0; i < l_max; ++i) {
        new_char_arr[i] = str[i];
    }

    free(str);

    return new_char_arr;
}

bool addEnt(char *str, entity *e) {

    int index = hash(str);
    printf("[DEBUG] indice dell'hash table calcolato su questo nome: %d\n", index);

    if(entity_hash[index] == NULL){
        //if the linked list in this hash index is empty, the entity is not present for sure
        printf("entro nell'indice dell'hash table %d che ho trovato vuoto.", index);
        entity * newEnt;
        newEnt = (entity *) malloc(sizeof(entity));

        newEnt->id_ent = (char*) malloc(sizeof(str));
        strcpy(newEnt->id_ent, str);
        newEnt->next = NULL;

        entity_hash[index] = newEnt;


        printf("[DEBUG] aggiunta entità: %s nell'indice %d che era vuoto (no collisioni). ", str, index);
        return true;

    } else {

        entity *ptr = entity_hash[index];

        do{
            if(strcmp(str, ptr->id_ent)==0){
                //the entity has already been added, do nothing
                printf("[DEBUG] entità già presente nella lista. non faccio nulla");
                return false;
            }
            ptr = ptr->next;
        }while(ptr->next != NULL);

        //at this point the entity has not been found and at the same time ptr points to the last pos of the linked list
        entity * newEnt;
        newEnt = (entity *) malloc(sizeof(entity));

        newEnt->id_ent = (char*) malloc(sizeof(str));
        strcpy(newEnt->id_ent, str);

        ptr->next = newEnt;
        newEnt->next = NULL;

        printf("[DEBUG] aggiunta entità: %s nell'indice %d che non era vuoto (si collisioni). ", str, index);
        return true;

    }


}

//hash function that generates an array index based on a calc on the entity name
int hash(char *str){

    long sum = 0;

    for (int i = 0; i < strlen(str); ++i) {

        sum = sum + (int) str[i];

    }

    return (sum + 3) % DEF_ENT_N;
}


bool delEnt(char *str, entity *entity_arr){

    //TODO

    return false;

}

bool addRel(){

    //TODO

    return false;
}

bool delRel(){

    //TODO

    return false;
}

void report(){

    //TODO
}
