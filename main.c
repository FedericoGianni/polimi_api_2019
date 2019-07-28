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
#define DEF_ENT_N 100
//entity dynamic array initial length
#define DEF_ENT_L 12

//initial relations type array length
#define DEF_REL_T_L 12
//relation hash table length
#define DEF_REL_N 1024


//GLOBAL DATA STRUCTURES

typedef struct entity {

    //pointer to a dynamic array which stores the id of the entity
    char *id_ent;

    //counters to keep track of the receiving relation for each entity
    int rel_ent_counters[DEF_REL_T_L];

    struct entity *next;

} entity;

typedef struct relation{

    //pointer to the entity who is giving the relation
    entity *sender;

    //pointer to the entity who is receiving the relation
    entity *receiving;

    struct relation *next_a;

    struct relation *next_b;


}relation;


typedef struct relation_t {

    //pointer to a dynamic array which stores the id of the relation type
    char *id_rel;

    //pointer to a dynamic array of pointers to the entities which have the max incoming relationships for this relation type
    entity *max_inc_rels;

    //store the current max number of receiving relations for this type
    int max;

    //pointer to a dynamic array of relation, which stores every single relation of this type
    relation *relation_hash[DEF_REL_N];


} relation_t;

//GLOBAL VARIABLES

//hash table to store the entities
entity *entity_hash [DEF_ENT_N];

//static array to store the relation_types
relation_t *relation_t_array[DEF_REL_T_L];

//FUNCTIONS DEFINITION
char *inputString(FILE *,int);

int hash(char *);

bool addEnt(char *, entity*);
bool delEnt(char *str, entity *entity_arr);
bool addRel(char *id_a, char *id_b, char *id_rel);
void report();


void main() {

    bool end = false;

    //dynamic array to read input lines
    char *input;

    //input without "" and other shit
    char *short_name;

    while(!end){

        //read input from stdin, line by line allocating a dynamic array for each line
        input = inputString(stdin, DEF_INPUT_L);

        printf("\n[DEBUG] Letta la stringa: %s", input);


        if(strncmp(input, ADDENT, 6) == 0){

            short_name = (char *) malloc((strlen(input) -10)* sizeof(char));
            strncpy(short_name, input + 8 * sizeof(char), strlen(input) - 9);
            short_name[strlen(input)-9] = '\0';
            printf("\n[DEBUG] Short_name: %s", short_name);

            printf("\n[DEBUG]----------------- chiamo addEnt per aggiungere un entità-------------");
            addEnt(short_name, entity_hash[0]);

            free(short_name);

        } else if(strncmp(input, DELENT, 6) == 0){
            //TODO remove this entity also from every relation
            printf("[DEBUG] read deleent\n");
            short_name = (char *) malloc((strlen(input) -10)* sizeof(char));
            strncpy(short_name, input + 7 * sizeof(char), strlen(input) - 7);
            printf("\n[DEBUG] Short_name: %s", short_name);
            delEnt(short_name, entity_hash[0]);


        } else if(strncmp(input, ADDREL, 6) == 0){
            printf("[DEBUG] read add rel\n");

            //TODO tirare fuori dalla stringa completa: id_rel, id_ent_A, id_ent_B
            //TODO addRel(id_rel, id_a, id_b);
            int curr_pos = 8;

            char *ch = &input[8];
            int i = 0;
            while(*ch != '"'){
                ch++;
                i++;
            }

            //printf("\ngiro1 i: %d, curr pos: %d", i,curr_pos);
            //printf("\nadesso ch punta alle virgolette -> %c", *ch);

            //+1 to include \0
            char *id_a = (char *) malloc(i* sizeof(char) + 1);
            strncpy(id_a, &input[8], i);
            id_a[i] = '\0';

            //skip the " " to point to id_b
            ch = &input[i+3];
            curr_pos += i + 3;
            //printf("\nadesso ch punta all'iniziale del 2 nome -> %c", *ch);

            //printf("giro1.1: %d, curr pos: %d", i,curr_pos);

            i = 0;
            while(*ch != '"'){
                ch++;
                i++;
            }

            //printf("\ngiro2 i: %d, curr pos: %d", i,curr_pos);
            //printf("\nadesso ch punta alle virgolette del 2 nome -> %c", *ch);

            //+1 to include \0
            char *id_b = (char *) malloc((i* sizeof(char)) + 1);
            strncpy(id_b, &input[curr_pos], i);
            id_b[curr_pos+i] = '\0';

            //skip the " " to point to id_b
            curr_pos += i + 3;
            ch = &input[curr_pos];

            //printf("\ngiro2.2: %d, curr pos: %d", i,curr_pos);
            //printf("\nadesso dovrebbe puntare alla a di amico: %c", input[curr_pos]);
            //printf("\nanche ch* dovrebbe puntare alla a di amico: %c", *ch);

            i = 0;
            while(*ch != '"'){
                ch++;
                i++;
            }


            //+1 to include \0
            char *id_rel = (char *) malloc((i* sizeof(char)) + 1);
            strncpy(id_rel, &input[curr_pos], i);
            id_rel[curr_pos+i-1] = '\0';

            printf("id_a: %s ", id_a);
            printf("id_b: %s ", id_b);
            printf("id_rel: %s ", id_rel);


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
        printf("\n\nentro nell'else!");
        entity *ptr = entity_hash[index];

        printf("ptr-> id: %s", ptr->id_ent);

        while(ptr->next != NULL){
            if(strcmp(str, ptr->id_ent)==0){
                //the entity has already been added, do nothing
                printf("[DEBUG] entità già presente nella lista. non faccio nulla");
                return false;
            }
            ptr = ptr->next;

        }

        //at this point the entity has not been found and at the same time ptr points to the last pos of the linked list
        printf("[DEBUG] collisione! aggiungo alla lista.");
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


bool delEnt(char *str, entity *e){

    //1. cerca l'entità da eliminare cercandola con l'hash
    //2. una volta trovata verifico se nella linked list corrispondente a quell'indice dell'hash table
    //   c'è dentro o no il nome da eliminare. se non c'è non faccio nulla
    //3. se il nome c'è -> 1. elimino tutte le relazioni che hanno a che fare con quel nome
    //                     2. aggiorno i vari contatori / lista di max per il report
    //                     3. elimino l'entità dalla hash table
    int index = hash(str);
    printf("\ndelent: elimina %s -> index: %d", str, index);

    entity *ptr = entity_hash[index];
    entity *prv = entity_hash[index];

    do{
        if(strcmp(str, ptr->id_ent)==0){
            //found entity to remove
            printf("\n[DEBUG] entità da rimuovere trovata!");
            //TODO scorri tutte le relazioni cercando quelle in cui l'utente è il datore o ricevente e eliminale
            //TODO aggiorna i contatori e la lista dei max
            //TODO rimuovi l'entita dalla hash table delle entità

            //3 casi
            //A) l'entità da eliminare è il primo della lista
            //B) l'entità da eliminare è in mezzo alla lista
            //c) l'entità da eliminare è in fondo alla lista


            if(ptr->id_ent == entity_hash[index]->id_ent){
                printf("entità da eliminare a cui sto puntando: %s", ptr->id_ent);
                if(ptr-> next != NULL) {
                    entity_hash[index] = ptr->next;
                    free(ptr);
                }
                //else
                    //entity_hash[index] = NULL;
                //free(ptr);
            } else{
                prv->next = ptr->next;
                free(ptr);
            }

            return true;

        } else {
            prv = ptr;
            ptr = ptr->next;
        }

    }while(ptr->next != NULL);

    //se non trova entità da eliminare non fa nulla
    printf("\n[DEBUG] Entità da rimuovere non trovata!");
    return false;

}

bool addRel(char *id_a, char *id_b, char *id_rel){

    // 1.1 verifica che id_rel esista già o meno -> se non esiste creala
    // 1.2 verifica che esistano entrambe le entità -> datore e ricevente
    // 2 aggiungi la relazione alla lista di relazioni di quel tipo
    // 3 aggiorna contatori entità e lista max

    //1.1
    int i = 0;
    bool found = false;

    for (i = 0; i < DEF_REL_T_L; ++i) {

        if(relation_t_array[i]->id_rel == NULL){
            //non c'è nessun tipo di relazione
            break;

        } else {

            if(strcmp(id_rel, relation_t_array[i]->id_rel) == 0){
                found = true;
                break;
            }
        }

    }

    //1.2 controllo che esistano entità A e B, se una delle 2 non esiste, termina
    bool entity_a_found = false;
    bool entity_b_found = false;
    int ind = hash(id_a);

    entity *ptr = entity_hash[ind];

    do{
        if(strcmp(id_a, ptr->id_ent)==0){
            entity_a_found = true;
        }
        ptr = ptr->next;

    }while(ptr->next != NULL);

    ind = hash(id_b);
    ptr = entity_hash[ind];

    do{
        if(strcmp(id_b, ptr->id_ent)==0){
            entity_b_found = true;
        }
        ptr = ptr->next;

    }while(ptr->next != NULL);

    if(!entity_a_found || !entity_b_found){
        printf("[DEBUG] una delle due entità non è presenti -> non faccio nulla");
        return false;
    }

    //se a questo punto non ha trovato niente found è false, devo creare il tipo di relazione nell'array
    //altrimenti procedi

    if(!found){
        //inserisci in pos i dell'array di tipi di relazioni il nuovo tipo di relazione

        relation_t *newRelType;
        newRelType = (relation_t*) malloc(sizeof(relation_t));

        newRelType->id_rel = (char *) malloc(sizeof(id_rel));
        strcpy(newRelType->id_rel, id_rel);
        newRelType->max = 0;

        relation_t_array[i] = newRelType;
    }


    relation *newSingleRel;
    //TODO trova il puntatore all'entità A
    //TODO aggiungi relazione nell'array di relazione e aggiungi puntatore a entità A in sender
    //TODO trova il puntatore all'entità B
    //TODO aggiungi entità B in receiving
    //relation_t_array[i]->relation_hash[hash(id_a)]->receiving = ; //puntatore all'entità A


    //TODO aggiorna i contatori delle entità -> ricevente ce l'ho anche già il puntatore, basta aumentare contatore giusto
    //TODO aggiorna la lista max per questo tipo di relazione


    return false;
}

bool delRel(){

    // 1 verifica che ci sia una relazione tra le 2 entità A e B
    // 2.1 se non c'è -> non fa nulla
    // 2.2 se c'è -> elimina la relazione; aggiorna contatori entità e lista max
    //          2.2B se era l'ultima relazione rimasta di quel tipo, cancella anche il tipo di relazione

    //TODO

    return false;
}

void report(){

    printf("[DEBUG] Lista entità aggiunte: ");
    entity *tmp;

    for (int i = 0; i < DEF_ENT_N; ++i) {

        //printf("\nhash[%d]", i);

        if(entity_hash[i] != NULL) {
            tmp = entity_hash[i];

            while (tmp->next != NULL) {
                if (tmp->id_ent != NULL) {
                    printf("%s", tmp->id_ent);
                    tmp = tmp->next;
                }

            }

            printf("%s", tmp->id_ent);
        }
    }
}
