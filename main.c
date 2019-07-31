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
#define DEF_INPUT_L 100
#define DEF_SHORT_NAME_L 90
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

//size for the array of entities who has max inc rel
#define DEF_MAX_REL_L 12


//GLOBAL DATA STRUCTURES

typedef struct entity {

    //pointer to a dynamic array which stores the id of the entity
    char *id_ent;

    //counters to keep track of the receiving relation for each entity
    int rel_ent_counters[DEF_REL_T_L];

    struct entity *next;

} entity;

//struct to store the entities who has max inc relations in a list inside every relation type
typedef struct max_entity {

    entity * ent_ptr;

    struct max_entity *next;

} max_entity;

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

    //pointer to a linked list of pointers to the entities which have the max incoming relationships for this relation type
    //TODO bisogna fare gli inserimenti in ordine alfabetico cosi resta ordinato per il report
    max_entity *max_entity_list;

    //store the current max number of receiving relations for this type
    int max;

    //store the index in which the entities have saved this relation inside their counter array
    int index;

    //hash table hashed by sender name
    relation *relation_sender_hash[DEF_REL_N];

    //hash table hashed by receiver name
    relation *relation_receiver_hash[DEF_REL_N];

    struct relation_t *next;

} relation_t;

//GLOBAL VARIABLES

//hash table to store the entities
entity *entity_hash [DEF_ENT_N];

//static array to store the relation_types
//TODO mi conviene quasi fare le liste perchè ri ordinare l'array statico è troppo oneroso
//TODO sfruttare inserimento in ordine alfabetico che c'è già per la lista di max ent!
relation_t *relation_t_head;

//FUNCTIONS DEFINITION
char *inputString(FILE *,int);

int hash(char *);

void sort_rel_t_array();

bool addEnt(char *, entity*);
bool delEnt(char *str, entity *entity_arr);
bool addRel(char *id_a, char *id_b, char *id_rel);
bool delRel(char *id_a, char *id_b, char *id_rel);
void report();

//just for debugging
void print();


void main() {

    bool end = false;

    //dynamic array to read input lines
    char input[DEF_INPUT_L];

    char *input_b = &input[0];

    size_t buffer_size = DEF_INPUT_L;
    size_t *buff_size_ptr = &buffer_size;

    //input without "" and other shit
    char short_name[DEF_SHORT_NAME_L];

    while(!end){

        //read input from stdin, line by line allocating a dynamic array for each line
        getline(&input_b, buff_size_ptr, stdin);
        //input = inputString(stdin, DEF_INPUT_L);

        //printf("\n[DEBUG] Letta la stringa: %s", input);

        if(strncmp(input, ADDENT, 6) == 0){

            //short_name = (char *) malloc((strlen(input) -10)* sizeof(char));
            strncpy(short_name, &input[8], strlen(input) - 10);
            short_name[strlen(input)-10] = '\0';
            //printf("\n[DEBUG] Short_name: %s", short_name);

            //printf("\n[DEBUG]----------------- chiamo addEnt per aggiungere un entità-------------");
            addEnt(short_name, entity_hash[0]);

            //free(short_name);

        } else if(strncmp(input, DELENT, 6) == 0){
            //TODO remove this entity also from every relation
            //printf("[DEBUG] read deleent\n");
            //short_name = (char *) malloc((strlen(input) -10)* sizeof(char));
            strncpy(short_name, input + 7 * sizeof(char), strlen(input) - 7);
            //printf("\n[DEBUG] Short_name: %s", short_name);
            delEnt(short_name, entity_hash[0]);


        } else if(strncmp(input, ADDREL, 6) == 0){
            //printf("\n[DEBUG] read add rel\n");
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
            strncpy(id_a, &input[8], i+1);
            id_a[i] = '\0';
            //printf("\nid_a: %s", id_a);

            //skip the " " to point to id_b
            ch = &input[i+11];
            curr_pos += i + 3;
            //printf("\nadesso ch punta all'iniziale del 2 nome -> %c", *ch);

            //printf("\ngiro1.1: %d, curr pos: %d", i,curr_pos);

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
            id_b[i] = '\0';
            //printf("\nid_b: %s", id_b);

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
            id_rel[i] = '\0';

            //printf("\nid_a: %s ", id_a);
            //printf("id_b: %s ", id_b);
            //printf("id_rel: %s ", id_rel);

            addRel(id_a, id_b, id_rel);


        } else if(strncmp(input, DELREL, 6) == 0){
            //printf("[DEBUG] read del rel\n");

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
            strncpy(id_a, &input[8], i+1);
            id_a[i] = '\0';
            //printf("\nid_a: %s", id_a);

            //skip the " " to point to id_b
            ch = &input[i+11];
            curr_pos += i + 3;
            //printf("\nadesso ch punta all'iniziale del 2 nome -> %c", *ch);

            //printf("\ngiro1.1: %d, curr pos: %d", i,curr_pos);

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
            //printf("\nid_b: %s", id_b);

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

            //printf("\nid_a: %s ", id_a);
            //printf("id_b: %s ", id_b);
            //printf("id_rel: %s ", id_rel);

            //TODO delRel(id_rel, id_a, id_b);
            delRel(id_a, id_b, id_rel);

        } else if(strncmp(input, REPORT, 6) == 0){
            //printf("[DEBUG] read repo\n");
            //print();
            report();

        } else if(strncmp(input, END, 3) == 0){
            //printf("[DEBUG] end!");
            end = true;

        } else {
            perror("[DEBUG] invalid command read!");
            break;
        }

        //de-allocate heap memory used to store string input, since in the next execution the pointer will point to
        //the new string and it will be impossible to eliminate old string from heap

        //free(input);

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
    //printf("[DEBUG] indice dell'hash table calcolato su questo nome: %d\n", index);

    if(entity_hash[index] == NULL){
        //if the linked list in this hash index is empty, the entity is not present for sure
        //printf("entro nell'indice dell'hash table %d che ho trovato vuoto.", index);
        entity * newEnt;
        newEnt = (entity *) malloc(sizeof(entity));

        newEnt->id_ent = (char*) malloc(sizeof(char)*(strlen(str)+1));
        strcpy(newEnt->id_ent, str);
        strcat(newEnt->id_ent, "\0");
        newEnt->next = NULL;

        for (int i = 0; i < DEF_MAX_REL_L; ++i) {
            newEnt->rel_ent_counters[i] = 0;
        }

        entity_hash[index] = newEnt;


        //printf("[DEBUG] aggiunta entità: %s nell'indice %d che era vuoto (no collisioni). ", str, index);
        return true;

    } else {
        //printf("\n\nentro nell'else!");
        entity *ptr = entity_hash[index];

        //printf("ptr-> id: %s", ptr->id_ent);

        while(ptr->next != NULL){
            if(strcmp(str, ptr->id_ent)==0){
                //the entity has already been added, do nothing
                //printf("[DEBUG] entità già presente nella lista. non faccio nulla");
                return false;
            }
            ptr = ptr->next;

        }

        //at this point the entity has not been found and at the same time ptr points to the last pos of the linked list
        //printf("[DEBUG] collisione! aggiungo alla lista.");
        entity * newEnt;
        newEnt = (entity *) malloc(sizeof(entity));

        newEnt->id_ent = (char*) malloc(sizeof(char)*(strlen(str)+1));
        strcpy(newEnt->id_ent, str);
        strcat(newEnt->id_ent, "\0");

        ptr->next = newEnt;
        newEnt->next = NULL;

        for (int i = 0; i < DEF_MAX_REL_L; ++i) {
            newEnt->rel_ent_counters[i] = 0;
        }

        //printf("[DEBUG] aggiunta entità: %s nell'indice %d che non era vuoto (si collisioni). ", str, index);

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

    // 1.1 verifica che esistano entrambe le entità -> datore e ricevente (se non esiste uno dei 2 fermati)
    // 1.2 verifica che id_rel esista già o meno -> se non esiste creala
    // 2 aggiungi la relazione alla lista di relazioni di quel tipo
    // 3 aggiorna contatori entità e lista max

    //1.1 controllo che esistano entità A e B, se una delle 2 non esiste, termina
    bool entity_a_found = false;
    bool entity_b_found = false;
    entity* ent_a;
    entity* ent_b;
    int ind = hash(id_a);

    //printf("\n\nindex restituito da hash(%s): %d", id_a, ind);

    entity* ptr = entity_hash[ind];
    //printf("entity_hash[ind] contiene: %s", entity_hash[ind]->id_ent);
    //printf("ptr punta a: %s", ptr->id_ent);


    while(!entity_a_found && ptr != NULL){
        //printf("verifico che l'entità: %s corrisponda all'entità: %s", id_a, ptr->id_ent);
        if(strcmp(id_a, ptr->id_ent)==0){
           entity_a_found = true;
           //printf("[DEBUG] entità A trovata!");

            //save a pointer to ent_A for future use
            ent_a = ptr;
            //printf("[DEBUG] ent_a ptr -> %s", ent_a->id_ent);
            break;

        }

        if(ptr->next != NULL)
            ptr = ptr->next;
        else
            break;
    }

    if(!entity_a_found){
        //printf("[DEBUG] entità A non esistente -> non faccio nulla");
        return false;
    }



    ind = hash(id_b);
    ptr = entity_hash[ind];

    while(!entity_b_found && ptr != NULL){
        if(strcmp(id_b, ptr->id_ent)==0){
            entity_b_found = true;
            //printf("[DEBUG] entità B trovata!");

            //save a pointer to ent_B for future use
            ent_b = ptr;
            //printf("[DEBUG] ent_b ptr -> %s", ent_b->id_ent);
            break;
        }

        if(ptr->next != NULL)
            ptr = ptr->next;
        else
            break;

    }

    if(!entity_b_found){
        //printf("[DEBUG] entità B non esistente -> non faccio nulla");
        return false;
    }

    //printf("[DEBUG] Ho trovato entrambe le entità! posso procedere?");
    //return true;

    //1.2 verifico se il tipo di rel esiste già o meno, in caso lo creo

    int i = 0;
    bool found = false;

    relation_t *rel_t_p = relation_t_head;

    while(!found && rel_t_p != NULL) {

        if(strcmp(rel_t_p->id_rel, id_rel) == 0){
            i = rel_t_p->index;
            found = true;
            break;
        }
        i++;

        if(rel_t_p->next != NULL)
            rel_t_p = rel_t_p->next;
        else
            break;

    }

    //printf("\n[DEBUG] se ha trovato la relazione found: %d", found);

    //se a questo punto non ha trovato niente found è false, devo creare il tipo di relazione nell'array
    //altrimenti procedi

    //TODO verifica che la relazione non esista già-> in quel caso non fare nulla
    if(found) {
        //printf("\n\nCERCO SE MAGARI LA REL ESISTE GIA");
        bool alreadyRel = false;
        relation *rel_p = rel_t_p->relation_receiver_hash[hash(ent_b->id_ent)];

        while (rel_p != NULL) {
            if (strcmp(rel_p->sender->id_ent, ent_a->id_ent) == 0 &&
                strcmp(rel_p->receiving->id_ent, ent_b->id_ent) == 0) {
                alreadyRel = true;
                break;
            }

            if (rel_p->next_a != NULL)
                rel_p = rel_p->next_a;
            else
                break;
        }

        if (alreadyRel) {
            //printf("\n[DEBUG] Relazione già esistente! Non fare niente.");
            return false;
        }
    }

    if(!found){
        //inserisci in pos i dell'array di tipi di relazioni il nuovo tipo di relazione

        relation_t *newRelType;
        newRelType = (relation_t*) malloc(sizeof(relation_t));

        newRelType->id_rel = (char *) malloc(sizeof(char)*(strlen(id_rel)+1));
        strcpy(newRelType->id_rel, id_rel);
        strcat(newRelType->id_rel, "\0");
        newRelType->max = 0;
        newRelType->max_entity_list = NULL;
        newRelType->next = NULL;
        newRelType->index = i;

        if(relation_t_head == NULL)
            relation_t_head = newRelType;
        else{
            relation_t *rel_p = relation_t_head;
            relation_t *rel_p_prev = NULL;

            while(rel_p->next != NULL){
                if(strcmp(rel_p->id_rel, ent_b->id_ent) < 0){
                    rel_p_prev = rel_p;
                    rel_p = rel_p->next;
                } else {
                    break;
                }
            }

            //CASO BASE
            if(rel_p_prev != NULL) {
                newRelType->next = rel_p;
                rel_p_prev->next = newRelType;
                rel_p->next = NULL;
                //rel_p->next = newRelType;
            } else {
                //caso in cui c'è solo 1
                //TODO differenziare i casi da metterlo prima o dopo quell'unico che c'è?
                if(strcmp(rel_p->id_rel, ent_b->id_ent) < 0){
                    newRelType->next = NULL;
                    rel_p->next = newRelType;

                } else if(strcmp(rel_p->id_rel, ent_b->id_ent) > 0){
                    newRelType->next = rel_p;
                    rel_p->next = NULL;
                    relation_t_head = newRelType;

                }

            }

        }
        rel_t_p = newRelType;

        //TODO sta roba non dovrebbe piu servire -> check
        /*
        //initialize with default values to avoid errors
        for (int j = 0; j < DEF_MAX_REL_L; ++j) {
            relation_t_array[i]->max = 0;
        }*/

        //initialize hash tables all to null
        for (int j = 0; j < DEF_REL_N; ++j) {
            newRelType->relation_sender_hash[j] = NULL;
            newRelType->relation_receiver_hash[j] = NULL;
        }

        //sort the relation_type array in alphabetical order
        //sort_rel_t_array();
    }

    //2 aggiungo la singola relazione alle due hash table di singole relazioni divise per tipo
    relation *newSingleRel;
    newSingleRel = (relation*) malloc(sizeof(relation));
    newSingleRel->sender = ent_a;
    newSingleRel->receiving = ent_b;
    newSingleRel->next_a = NULL;
    newSingleRel->next_b = NULL;

    //i punta al nuovo relation_t o a quello trovato se c'era già

    int index_a = hash(id_a);
    //printf("[DEBUG] indice dell'hash table calcolato su questo nome: %d\n", index_a);

    if(rel_t_p->relation_receiver_hash[index_a] == NULL){
        //if the linked list in this hash index is empty, the entity is not present for sure
        //printf("entro nell'indice dell'hash table %d che ho trovato vuoto.", index_a);

        rel_t_p->relation_receiver_hash[index_a] = newSingleRel;

        //printf("[DEBUG] aggiunta relazione: %s nell'indice %d che era vuoto (no collisioni). ", id_rel, index_a);

    } else {
        //printf("----------------------------------------------------------entro nell'else!");

        relation *ptr_a = rel_t_p->relation_receiver_hash[index_a];

        while (ptr_a->next_a != NULL) {
            ptr_a = ptr_a->next_a;
        }

        ptr_a->next_a = newSingleRel;
        newSingleRel->next_a = NULL;
    }
    //printf("-----------------------esco dall'else");


    int index_b = hash(id_b);
    //printf("[DEBUG] indice dell'hash table calcolato su questo nome: %d\n", index_b);

    if(rel_t_p->relation_sender_hash[index_b] == NULL){
        //printf("entro nell'if");
        //if the linked list in this hash index is empty, the entity is not present for sure
        //printf("entro nell'indice dell'hash table %d che ho trovato vuoto.", index_b);

        rel_t_p->relation_receiver_hash[index_b] = newSingleRel;

        //printf("[DEBUG] aggiunta relazione: %s nell'indice %d che era vuoto (no collisioni). ", id_rel, index_b);

    } else {
        //printf("\n\nentro nell'else!");

        relation *ptr_b = rel_t_p->relation_receiver_hash[index_b];

        while (ptr_b->next_b != NULL) {
            ptr_b = ptr_b->next_b;
        }

        ptr_b->next_b = newSingleRel;
        //newSingleRel->next_b = NULL;
    }

    //aggiorna i contatori delle entità -> ricevente ce l'ho anche già il puntatore, basta aumentare contatore giusto
    ent_b->rel_ent_counters[i]++;
    //mette nel relation type l'integer a cui corrisponde nell'indice dell'array delle entità per questo tipo
    rel_t_p->index = i;
    //aggiorna il max del tipo di relazione se necessario
    if(ent_b->rel_ent_counters[i] > rel_t_p->max){
        //TODO aggiungerlo in ordine alfabetico
        //TODO in questo caso devo 1 cancellare quelli che c'erano prima 2 inserirlo (quindi non serve ordine alfab)
        rel_t_p->max = ent_b->rel_ent_counters[i];
        //cancellare tutte quelle che c'erano prima perchè è un nuovo max
        //relation_t_array[i]->max_inc_rels[i]->
        //inserirlo (in questo caso non serve ordine alfabetico

        //ent_b il nuovo max, cancella tutti quelli precendeti e aggiungi solo questo alla lista dei max
        max_entity *max_ptr = rel_t_p->max_entity_list;

        //elimina la vecchia lista dei max
        while(max_ptr != NULL){
            //TODO elimina la vecchia lista dei max
            //TODO ELIMINA LA VECHIA LISTA DEI MAX SENÒ CIAO LA MEMORIA POI... just for test
            max_ptr = max_ptr->next;
        }

        //inserisci il nuovo max in cima alla lista
        max_entity *newMaxEnt;
        newMaxEnt = (max_entity*) malloc(sizeof(max_entity));
        newMaxEnt->next = NULL;
        newMaxEnt->ent_ptr = ent_b;
        rel_t_p->max_entity_list = newMaxEnt;

    } else if(ent_b->rel_ent_counters[i] == rel_t_p->max) {
        //aggiorna la lista max per questo tipo di relazione
        //se non c'era nessun altro max inserisci in prima posizione
        if (rel_t_p->max_entity_list == NULL) {

            max_entity *newMaxEnt;
            newMaxEnt = (max_entity *) malloc(sizeof(max_entity));
            newMaxEnt->next = NULL;
            newMaxEnt->ent_ptr = ent_b;
            rel_t_p->max_entity_list = newMaxEnt;

            //relation_t_array[i]->max_inc_rels[0] = ent_b;

        } else {
            //ent_b è uguale al max, aggiungilo all'array senza cancellare gli altri -> però in ord alfab
            //scorri fino a quando non trovi il posto giusto e inseriscilo
            //TODO va inserito in ordine alfabetico per fare il report giusto
            //TODO inserisci in ordine alfabetico ent_b nella lista dei max
            //printf("\n\nINSERISCI IN ORDINE ALFABETICO DAI CAZOZ!!");
            //printf("\n[DEBUG] devo inserire in ordine alfabetico: %s", ent_b->id_ent);

            max_entity *max_ptr = rel_t_p->max_entity_list;
            max_entity *max_ptr_pre = NULL;
            int k = 0;

            while (max_ptr != NULL) {

                if(strcmp(max_ptr->ent_ptr->id_ent, ent_b->id_ent) < 0){
                    //printf("\n------------------------------------continua a proseguire!!");
                } else {
                    break;
                }


                k = 0;
                //printf("\n[DEBUG] contronto %s con %s", max_ptr->ent_ptr->id_ent, ent_b->id_ent);
                while((int) max_ptr->ent_ptr->id_ent[k] <= (int) ent_b->id_ent[k]){
                    k++;
                }

                //printf("\nk tra %s e %s", max_ptr->ent_ptr->id_ent, ent_b->id_ent);
                //printf("\n\nk: %d", k);


                //TODO confrontare solo il carattere k-esimo senza riconfrontare ogni volta tutta la sottostringa
                //TODO controlalre se va fatto > o <
                /*
                if(max_ptr_pre != NULL)
                    printf("\nmax_ptr_pre -> %s", max_ptr_pre->ent_ptr->id_ent);
                else
                    printf("\nmax_ptr_pre -> NULL");
                printf("\nmax_ptr -> %s", max_ptr->ent_ptr->id_ent);
                 */

                max_ptr_pre = max_ptr;
                max_ptr = max_ptr->next;



            }

            //adesso max_ptr punta al primo > e max_ptr_pre punta al primo minore
            //max_ptr_pre->next = quello da aggiungere
            //quello da aggiungere -> next = max_ptr

            max_entity *newMaxEnt;
            newMaxEnt = (max_entity *) malloc(sizeof(max_entity));

            if(k == 0){
                //inserisci prima
                rel_t_p->max_entity_list = newMaxEnt;
                newMaxEnt->next = max_ptr;
                newMaxEnt->ent_ptr = ent_b;

            } else {
                //inserisci dopo
                //TODO vedere se sto if else ha senso o no
                if(max_ptr_pre != NULL)
                    max_ptr_pre->next = newMaxEnt;
                else
                    rel_t_p->max_entity_list = newMaxEnt;
                newMaxEnt->next = max_ptr;
                newMaxEnt->ent_ptr = ent_b;
            }



            //TODO questo caso noon dovrebbe servire -> impossibile che sia gia nel max -> da verificare
            /*
            int j = 0;
            bool alreadyInMax = false;
            while(relation_t_array[i]->max_inc_rels[j] != NULL){
                if (strcmp(relation_t_array[i]->max_inc_rels[j]->id_ent, ent_b->id_ent) == 0) {
                    alreadyInMax = true;
                    break;
                }
                j++;
            }
        }
            printf("\n[DEBUG] LISTA ATTUALE dei max per la relazione: %s\n", relation_t_array[i]->id_rel);
            max_entity *p = relation_t_array[i]->max_entity_list;
            while(p != NULL){

                printf("%s , " ,p->ent_ptr->id_ent);
                p = p->next;
            }
            printf("\n------------------------");*/


        }
    }

    return true;
}

bool delRel(char *id_a, char *id_b, char *id_rel){

    // 1 verifica che ci sia una relazione tra le 2 entità A e B
    // 1.1 verificare che relazione esista
    // 1.2 verificare che entità A esista
    // 1.3 verificare che entità B esista
    // 1.4 verificare che la relazione esista
    // 2.1 se non c'è -> non fa nulla
    // 2.2 se c'è -> elimina la relazione; aggiorna contatori entità e lista max
    //          2.2B se era l'ultima relazione rimasta di quel tipo, cancella anche il tipo di relazione

    //TODO

    return false;
}

void report() {
    //format: esempi
    // "amico_di" "bruno" 2; "compagno_di" "dario" 1;
    // "amico_di" "bruno" 2; "compagno_di" "alice" "dario" 1;

    int i = 0;
    relation_t *rel_t_ptr = relation_t_head;

    if(relation_t_head == NULL){
        printf("none");
    }

    while(rel_t_ptr != NULL){
        if(i != 0){
            printf(" ");
        }

        printf("\"%s\" ", rel_t_ptr->id_rel);

        max_entity *ptr = rel_t_ptr->max_entity_list;
        while(ptr != NULL){
            printf("\"%s\" ", ptr->ent_ptr->id_ent);
            ptr = ptr->next;
        }

        printf("%d;", rel_t_ptr->max);

        rel_t_ptr = rel_t_ptr->next;
        i++;

    }

    printf("\n");


    /*
    for (i = 0; i < DEF_REL_T_L; ++i) {
        if (relation_t_array[i] != NULL) {
            none = false;
            if(i != 0){
                printf(" ");
            }

            printf("\"%s\" ", relation_t_array[i]->id_rel);
            int j = 0;
            max_entity *ptr = relation_t_array[i]->max_entity_list;
            while(ptr != NULL){
                printf("\"%s\" ", ptr->ent_ptr->id_ent);
                ptr = ptr->next;
            }
            printf("%d;", relation_t_array[i]->max);
        }*/

        //if(i < DEF_REL_T_L) {
         //   if (relation_t_array[i + 1] != NULL) {
          //      printf(" ");
           // }
        //}

    //}


    //TODO valutare caso in cui ci sono piu di un max con lo stesso numero di relazioni entranti
    /*
    int j = 0;
    while(relation_t_array[i]->max_inc_rels[j] != NULL){
        printf("\"%s\" %d;", relation_t_array[i]->max_inc_rels[j]->id_ent, relation_t_array[i]->max);
        j++;
    }*/

}

void print(){

    /*
    printf("[DEBUG] Lista entità aggiunte: ");
    entity *tmp;

    for (int i = 0; i < DEF_ENT_N; ++i) {

        //printf("\nhash[%d]", i);

        if(entity_hash[i] != NULL) {
            tmp = entity_hash[i];

            while (tmp->next != NULL) {
                if (tmp->id_ent != NULL) {
                    printf(" %s ", tmp->id_ent);
                    tmp = tmp->next;
                }

            }

            printf(" %s "
                   "", tmp->id_ent);
        }
    }*/
    /*
    for (int l = 0; l < DEF_REL_T_L; ++l) {

        if(relation_t_array[l] != NULL) {
            printf("\n[DEBUG] stato delle hash table del tipo di rel: %d: %s", l, relation_t_array[l]->id_rel);
            for (int k = 0; k < DEF_REL_N; ++k) {
                //scorri k-esima posizione della hash table
                if (relation_t_array[l]->relation_sender_hash[k] != NULL) {

                    relation* ptr = relation_t_array[l]->relation_receiver_hash[k];
                    while(ptr != NULL){
                        printf("\nhash_table_sender[%d]: ", k);
                        printf("\nid_datore: %s", ptr->sender->id_ent);
                        printf("\nid_ricevente: %s", ptr->receiving->id_ent);
                        ptr = ptr->next_a;
                    }
                }

                if (relation_t_array[l]->relation_receiver_hash[k] != NULL) {

                    relation *ptr = relation_t_array[l]->relation_receiver_hash[k];
                    while (ptr != NULL) {
                        printf("\nhash_table_receiver[%d]: ", k);
                        printf("\nid_datore: %s", ptr->sender->id_ent);
                        printf("\nid_ricevente: %s", ptr->receiving->id_ent);
                        ptr = ptr->next_b;
                    }
                }

            }
        }
    }*/
    /*
    for (int l = 0; l < DEF_REL_T_L; ++l) {
        if(relation_t_array[l] != NULL) {
            max_entity *ptr = relation_t_array[l]->max_entity_list;
            printf("\n[DEBUG] scorro la lista entità max per il tipo di rel: %d: %s", l, relation_t_array[l]->id_rel);
            while (ptr != NULL) {
                printf("\nptr -> %s", ptr->ent_ptr->id_ent);
                ptr = ptr->next;
            }
        }
    }*/
}
/*
void sort_rel_t_array(){

    //insertion sort -> could be improved but it wont impact that much because there are just a few rel_types
    //if need little time improvement could improve this function

    //printf("\n------------------------[DEBUG] chiamato sort relation type array!--------------------------");

    int i = 1;
    while (i < DEF_REL_T_L){

        int j = i;

        //there is only 1 element->already sorted
        if(relation_t_array[j] == NULL || relation_t_array[j-1] == NULL){
            return;
        }


        printf("\n--------------INITIAL--------------");
        printf("\n\trelation_t_array[0]->id_rel: %s", relation_t_array[0]->id_rel);
        printf("\n\trelation_t_array[1]->id_rel: %s\n", relation_t_array[1]->id_rel);



        //k è l'indice della prima lettera diversa per le 2 relazioni che sto confrontando
        int k = 0;

        while(k < strlen(relation_t_array[j-1]->id_rel) && k < strlen(relation_t_array[j]->id_rel)){
            if ((int) relation_t_array[j - 1]->id_rel[k] == (int) relation_t_array[j]->id_rel[k]) {
                k++;
            } else {
                break;
            }
        }


        while(j > 0 && ((int) relation_t_array[j-1]->id_rel[k] > (int) relation_t_array[j]->id_rel[k])){

        //TODO per il futuro ricordarsi che in caso necessito di ottimizzazioni posso sostituire strcmp
        //TODO facendo i confronti carattere per carattere
        while(j > 0 && strcmp(relation_t_array[j-1]->id_rel, relation_t_array[j]->id_rel) > 0){
            //swap A[j] and A[j-1]

            relation_t* temp;
            temp = (relation_t*) malloc(sizeof(relation_t));

            temp->id_rel = relation_t_array[j]->id_rel;
            temp->max_entity_list = relation_t_array[j]->max_entity_list;
            temp->max = relation_t_array[j]->max;
            temp->index = relation_t_array[j]->index;

            relation_t_array[j] = relation_t_array[j-1];
            relation_t_array[j-1]->max = temp->max;

            j--;
        }


        printf("\n----------SWAP-----------");
        printf("\n\trelation_t_array[0]->id_rel: %s", relation_t_array[0]->id_rel);
        printf("\n\trelation_t_array[1]->id_rel: %s\n", relation_t_array[1]->id_rel);


        i++;
    }
}*/


