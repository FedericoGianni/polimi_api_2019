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
#define DEF_SHORT_NAME_L 50
//input length increment
#define DEF_INPUT_L_INC 16

//entity hash table length
#define DEF_ENT_N 999910
//entity dynamic array initial length
#define DEF_ENT_L 12

//initial relations type array length
#define DEF_REL_T_L 12
//relation hash table length
#define DEF_REL_N 9991

//size for the array of entities who has max inc rel
#define DEF_MAX_REL_L 12

#define DEF_DEL_REL_T_IND_L 5

#define DEF_OUTPUT_L 10000

//GLOBAL DATA STRUCTURES
char virgoletta[2] = "\"";
char virgolettaspazio[3] = "\" ";
char puntoevirgola[2] = ";";
char acapo[2] = "\n";
char vuoto[1] = {"\0"};
char max[2];

typedef struct entity
{

    //pointer to a dynamic array which stores the id of the entity
    char *id_ent;

    char id_ent_n[DEF_SHORT_NAME_L];

    //counters to keep track of the receiving relation for each entity
    int rel_ent_counters[DEF_REL_T_L];

    struct entity *next;

} entity;

//struct to store the entities who has max inc relations in a list inside every relation type
typedef struct max_entity
{

    entity * ent_ptr;

    struct max_entity *next;

} max_entity;

typedef struct relation
{

    //pointer to the entity who is giving the relation
    entity *sender;

    //pointer to the entity who is receiving the relation
    entity *receiving;

    struct relation *next_a;

    struct relation *next_b;


}relation;

typedef struct relation_t
{
    //static array (faster that dynamic) to store the id of the relation type
    char id_rel[DEF_SHORT_NAME_L];

    //pointer to a linked list of pointers to the entities which have the max incoming relationships for this relation type
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

typedef struct rem_rel_t{

    relation *p_a_pre;
    relation *p_a_next;

    relation *p_b_pre;
    relation *p_b_next;

    //true if the current rem_rel exection has aslo deleted the rel_type
    //bool deletedRelType;

}rem_rel_t;

//GLOBAL VARIABLES

//hash table to store the entities
entity *entity_hash [DEF_ENT_N];

//return type for rem_rel function
rem_rel_t rem_rel_ret;


//static array to store the relation_types
relation_t *relation_t_head;

//array to store index of the deleted relations to reuse them
int deleted_rel_type_indexes[DEF_DEL_REL_T_IND_L];

//FUNCTIONS DEFINITION
int hash_n(char *);
int hash_r(char *);

rem_rel_t rem_rel(relation_t*, relation_t*, relation *, relation *, relation*, relation *, int, int);

bool addEnt(char *, entity*);
bool delEnt(char *str, entity *entity_arr);
bool addRel(char *id_a, char *id_b, char *id_rel);
bool delRel(char *id_a, char *id_b, char *id_rel);
void report();

//just for debugging
void print();


void main()
{

    bool end = false;

    //dynamic array to read input lines
    char input[DEF_INPUT_L];

    char *input_b = &input[0];

    size_t buffer_size = DEF_INPUT_L;
    size_t *buff_size_ptr = &buffer_size;

    //input without "" and other shit
    char short_name[DEF_SHORT_NAME_L];

    //deleted_rel_type_array initializatio
    for (int j = 0; j < DEF_DEL_REL_T_IND_L; ++j)
    {
        deleted_rel_type_indexes[j] = -1;
    }

    while(!end)
    {

        //read input from stdin, line by line allocating a dynamic array for each line
        int r = getline(&input_b, buff_size_ptr, stdin);
        //input = inputString(stdin, DEF_INPUT_L);

        //printf("\n[DEBUG] Letta la stringa: %s", input);

        if(strncmp(input, ADDENT, 6) == 0)
        {

            strncpy(short_name, &input[8], strlen(input) - 10);
            short_name[strlen(input)-10] = '\0';
            //printf("\n[DEBUG] Short_name: %s", short_name);

            addEnt(short_name, entity_hash[0]);

        } else if(strncmp(input, DELENT, 6) == 0){

            strncpy(short_name, &input[8], strlen(input) - 10);
            short_name[strlen(input)-10] = '\0';
            //printf("\n[DEBUG] Short_name: %s", short_name);
            //printf("\n[DEBUG] DELETE: %s", short_name);

            delEnt(short_name, entity_hash[0]);

        } else if(strncmp(input, ADDREL, 6) == 0){

            int curr_pos = 8;

            char *ch = &input[8];
            int i = 0;
            while(*ch != '"'){
                ch++;
                i++;
            }


            //+1 to include \0
            char id_a[DEF_SHORT_NAME_L];
            strncpy(id_a, &input[8], i+1);
            id_a[i] = '\0';
            //printf("\nid_a: %s", id_a);

            //skip the " " to point to id_b
            ch = &input[i+11];
            curr_pos += i + 3;

            i = 0;
            while(*ch != '"'){
                ch++;
                i++;
            }

            //+1 to include \0
            char id_b[DEF_SHORT_NAME_L];
            strncpy(id_b, &input[curr_pos], i+1);
            id_b[i] = '\0';
            //printf("\nid_b: %s", id_b);

            //skip the " " to point to id_b
            curr_pos += i + 3;
            ch = &input[curr_pos];

            i = 0;
            while(*ch != '"'){
                ch++;
                i++;
            }

            char id_rel[DEF_SHORT_NAME_L];
            strncpy(id_rel, &input[curr_pos], i);
            id_rel[i] = '\0';

            /*
            printf("\n[DEBUG] ADDREL: ");
            printf("\nid_a: %s ", id_a);
            printf("id_b: %s ", id_b);
            printf("id_rel: %s ", id_rel);
            */

            addRel(id_a, id_b, id_rel);


        } else if(strncmp(input, DELREL, 6) == 0)
        {

            int curr_pos = 8;

            char *ch = &input[8];
            int i = 0;
            while(*ch != '"')
            {
                ch++;
                i++;
            }

            //+1 to include \0
            char id_a[DEF_SHORT_NAME_L];
            strncpy(id_a, &input[8], i+1);
            id_a[i] = '\0';
            //printf("\nid_a: %s", id_a);

            //skip the " " to point to id_b
            ch = &input[i+11];
            curr_pos += i + 3;

            i = 0;
            while(*ch != '"')
            {
                ch++;
                i++;
            }

            //+1 to include \0
            char id_b[DEF_SHORT_NAME_L];
            strncpy(id_b, &input[curr_pos], i+1);
            id_b[i] = '\0';
            //printf("\nid_b: %s", id_b);

            //skip the " " to point to id_b
            curr_pos += i + 3;
            ch = &input[curr_pos];

            i = 0;
            while(*ch != '"')
            {
                ch++;
                i++;
            }

            //+1 to include \0
            char id_rel[DEF_SHORT_NAME_L];
            strncpy(id_rel, &input[curr_pos], i);
            id_rel[i] = '\0';

            /*
            printf("\n[DEBUG] DELREL: ");
            printf("\nid_a: %s ", id_a);
            printf("id_b: %s ", id_b);
            printf("id_rel: %s ", id_rel);
             */

            delRel(id_a, id_b, id_rel);

        } else if(strncmp(input, REPORT, 6) == 0){
            //print();
            report();

        } else if(strncmp(input, END, 3) == 0){
            end = true;

        } else {
            perror("[DEBUG] invalid command read!");
            break;
        }
    }
}

bool addEnt(char *str, entity *e) {

    int index = hash_n(str);
    //printf("[DEBUG] indice dell'hash table calcolato su questo nome: %d\n", index);

    if(entity_hash[index] == NULL){
        //if the linked list in this hash index is empty, the entity is not present for sure
        //printf("entro nell'indice dell'hash table %d che ho trovato vuoto.", index);
        entity * newEnt;
        newEnt = (entity *) malloc(sizeof(entity));


        //newEnt->id_ent = (char*) malloc(sizeof(char)*(strlen(str)+1));
        newEnt->id_ent = newEnt->id_ent_n;
        strcpy(newEnt->id_ent, str);
        strcat(newEnt->id_ent, "\0");
        newEnt->next = NULL;

        for (int i = 0; i < DEF_REL_T_L; i++) {
            newEnt->rel_ent_counters[i] = 0;
        }

        entity_hash[index] = newEnt;

        //printf("[DEBUG] aggiunta entità: %s nell'indice %d che era vuoto (no collisioni). ", str, index);
        return true;

    } else {
        //printf("\n\nentro nell'else!");
        entity *ptr = entity_hash[index];

        //printf("ptr-> id: %s", ptr->id_ent);

        while(ptr != NULL){
            if(strcmp(str, ptr->id_ent)==0){
                //the entity has already been added, do nothing
                //printf("[DEBUG] entità già presente nella lista. non faccio nulla");
                return false;
            }

            if(ptr->next != NULL)
                ptr = ptr->next;
            else
                break;

        }

        //at this point the entity has not been found and at the same time ptr points to the last pos of the linked list
        //printf("[DEBUG] collisione! aggiungo alla lista.");
        entity * newEnt;
        newEnt = (entity *) malloc(sizeof(entity));

        newEnt->id_ent = newEnt->id_ent_n;
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

//hash function
int hash_r ( char *s )
{
    unsigned long   h = 0, high;
    while ( *s )
    {
        h = ( h << 4 ) + *s++;
        if ( high = h & 0xF0000000 )
            h ^= high >> 24;
        h &= ~high;
    }
    return h % DEF_REL_N;
}
//hash 5 let's try
int hash_n ( char *s )
{
    unsigned long   h = 0, high;
    while ( *s )
    {
        h = ( h << 4 ) + *s++;
        if ( high = h & 0xF0000000 )
            h ^= high >> 24;
        h &= ~high;
    }
    return h % DEF_ENT_N;
}

bool addRel(char *id_a, char *id_b, char *id_rel)
{

    // 1.1 verifica che esistano entrambe le entità -> datore e ricevente (se non esiste uno dei 2 fermati)
    // 1.2 verifica che id_rel esista già o meno -> se non esiste creala
    // 1.3 verifica che la relazioen non esista già -> se esiste gia non fare nulla
    // 2 aggiungi la relazione alla lista di relazioni di quel tipo
    // 3 aggiorna contatori entità e lista max

    //1.1 controllo che esistano entità A e B, se una delle 2 non esiste, termina
    bool entity_a_found = false;
    bool entity_b_found = false;
    entity *ent_a;
    entity *ent_b;
    int ind_a = hash_n(id_a);

    entity *ptr = entity_hash[ind_a];

    while (!entity_a_found && ptr != NULL)
    {
        //printf("verifico che l'entità: %s corrisponda all'entità: %s", id_a, ptr->id_ent);
        if (strcmp(id_a, ptr->id_ent) == 0)
        {
            entity_a_found = true;

            //save a pointer to ent_A for future use
            ent_a = ptr;
            break;

        }

        if (ptr->next != NULL)
            ptr = ptr->next;
        else
            break;
    }

    if (!entity_a_found)
    {
        return false;
    }


    int ind_b = hash_n(id_b);
    ptr = entity_hash[ind_b];

    while (!entity_b_found && ptr != NULL)
    {
        if (strcmp(id_b, ptr->id_ent) == 0)
        {
            entity_b_found = true;

            //save a pointer to ent_B for future use
            ent_b = ptr;
            break;
        }

        if (ptr->next != NULL)
            ptr = ptr->next;
        else
            break;

    }

    if (!entity_b_found)
    {
        return false;
    }

    //1.2 verifico se il tipo di rel esiste già o meno, in caso negativo lo creo

    int i = 0;
    bool found = false;

    relation_t *rel_t_p = relation_t_head;

    while (!found && rel_t_p != NULL)
    {

        if (strcmp(rel_t_p->id_rel, id_rel) == 0)
        {
            found = true;
            break;

        } else
        {
            i++;
            if (!found && rel_t_p->next != NULL) {
                rel_t_p = rel_t_p->next;
            }
            else{
                break;
            }
        }
    }

    /*
    if(!found)
        printf("\n[DEBUG] Relazione non trovata i della rel (nuova) %s: %d", id_rel, i);
    else
        printf("\n[DUBUG] Trovata: Indice della relazione %s : %d", rel_t_p->id_rel, rel_t_p->index);
    */

    //se a questo punto non ha trovato niente found è false, devo creare il tipo di relazione nell'array
    //altrimenti procedi

    // 1.3 verifica che la relazione non esista già-> in quel caso non fare nulla
    if (found)
    {

        bool alreadyRel = false;
        int h = hash_r(ent_b->id_ent);
        relation *rel_p = rel_t_p->relation_receiver_hash[h];

        while (!alreadyRel && rel_p != NULL)
        {
            //printf("\nverifico se esiste una corrispondenza per la rel ent_a: %s, ent_b: %s", ent_a->id_ent, ent_b->id_ent);

            if (strcmp(rel_p->sender->id_ent, ent_a->id_ent) == 0 &&
                strcmp(rel_p->receiving->id_ent, ent_b->id_ent) == 0)
            {
                alreadyRel = true;
                break;

            } else
            {
                if (rel_p->next_b != NULL)
                    rel_p = rel_p->next_b;
                else
                    break;
            }
        }

        if (alreadyRel)
        {
            return false;
        }

    } else
    {
        //printf("\n[DEBUG] Inserisco la nuova relazione nella lista di relation type.");
        //inserisci in pos i dell'array di tipi di relazioni il nuovo tipo di relazione

        relation_t *newRelType;
        newRelType = (relation_t *) malloc(sizeof(relation_t));

        //initialize hash tables all to null
        for (int j = 0; j < DEF_REL_N; ++j)
        {
            newRelType->relation_sender_hash[j] = NULL;
            newRelType->relation_receiver_hash[j] = NULL;
        }

        strcpy(newRelType->id_rel, id_rel);
        strcat(newRelType->id_rel, "\0");
        newRelType->max = 0;
        newRelType->max_entity_list = NULL;
        newRelType->next = NULL;

        newRelType->index = i;

        relation_t *rel_t_ptr;
        relation_t *rel_t_ptr_prev;

        if (relation_t_head == NULL)
        {
            //CASO A -> se è nuovo
            relation_t_head = newRelType;
            newRelType->next = NULL;
            //printf("\n CASO A -> relation_t_head = newReltype");
        } else
        {
            rel_t_ptr = relation_t_head;
            rel_t_ptr_prev = NULL;
            bool stop = false;

            while (!stop && rel_t_ptr != NULL)
            {

                if (strcmp(id_rel, rel_t_ptr->id_rel) > 0)
                {
                    stop = true;
                    //break;
                }


                if (rel_t_ptr->next != NULL)
                {
                    rel_t_ptr_prev = rel_t_ptr;
                    rel_t_ptr = rel_t_ptr->next;
                } else {
                    break;
                }

            }

            //INSERISCO TIPO DI RELAZIONE IN ORDINE
            //prev->next = nuovo / nuovo -> next = ptr
            /*
            printf("\n[DEBUG] devo inserire rel_type in ordine.\n rel_t_ptr->%s", rel_t_ptr->id_rel);
            if(rel_t_ptr_prev != NULL)
                printf("\nrel_t_ptr_pre->%s\n\n", rel_t_ptr_prev->id_rel);
            else
                printf("\nrel_t_ptr_prev == NULL\n\n");
                */

            //CASO B -> c'è solo un rel_type al momento

            if (rel_t_ptr != NULL && rel_t_ptr_prev == NULL) {
                //printf("\nCASO B -> c'è solo 1 rel_type al momento");

                if (strcmp(id_rel, rel_t_ptr->id_rel) > 0) {
                    //printf("\nCOMPARA %s con %s = %d", rel_t_ptr->id_rel, id_rel, strcmp(rel_t_ptr->id_rel, id_rel));
                    if (strcmp(id_rel, rel_t_ptr->id_rel) < 0) {
                        newRelType->next = relation_t_head;
                        relation_t_head = newRelType;
                    } else {
                        newRelType->next = NULL;
                        rel_t_p->next = newRelType;
                    }

                } else {
                    newRelType->next = rel_t_ptr;
                    relation_t_head = newRelType;
                    relation_t_head->next = rel_t_ptr;
                }

            } else if (rel_t_ptr != NULL && rel_t_ptr_prev != NULL) {
                //printf("\n CASO C -> ci sono almeno 2 relation type");
                if (strcmp(rel_t_ptr->id_rel, newRelType->id_rel) > 0) {
                    //ins prima
                    rel_t_ptr_prev->next = newRelType;
                    newRelType->next = rel_t_ptr;
                } else {
                    //ins dopo
                    rel_t_p->next = newRelType;
                    newRelType->next = NULL;
                }

            } else {
                printf("\n[DEBUG] ATTENZIONE c'è un caso non considerato nell'inserimento del rel_type!!");
            }

        }

            //assegna a rel_t_p nuovo valore per dopo
            rel_t_p = newRelType;
    }


    //2 aggiungo la singola relazione alle due hash table di singole relazioni divise per tipo
    relation *newSingleRel;
    newSingleRel = (relation *) malloc(sizeof(relation));
    newSingleRel->sender = ent_a;
    newSingleRel->receiving = ent_b;
    newSingleRel->next_a = NULL;
    newSingleRel->next_b = NULL;

    //i punta al nuovo relation_t o a quello trovato se c'era già

    int index_a = hash_r(id_a);

    if (rel_t_p->relation_sender_hash[index_a] == NULL)
    {
        //if the linked list in this hash index is empty, the entity is not present for sure
        rel_t_p->relation_sender_hash[index_a] = newSingleRel;

        //printf("\n[DEBUG] aggiunta relazione in H_A: %s nell'indice %d che era vuoto (no collisioni). ", id_rel, index_a);

    } else
    {

        //inserisci in testa
        relation **ptr_a = &rel_t_p->relation_sender_hash[index_a];
        newSingleRel->next_a = *ptr_a;
        *ptr_a = newSingleRel;

        //printf("\n[DEBUG] aggiunta relazione in H_A: %s nell'indice %d che non era vuoto (si collisioni). ", id_rel, index_a);
    }


    int index_b = hash_r(id_b);
    //printf("[DEBUG] indice dell'hash table calcolato su questo nome: %d\n", index_b);

    if (rel_t_p->relation_receiver_hash[index_b] == NULL) {
        //if the linked list in this hash index is empty, the entity is not present for sure
        rel_t_p->relation_receiver_hash[index_b] = newSingleRel;

        //printf("[DEBUG] aggiunta relazione in H_B: %s nell'indice %d che era vuoto (no collisioni). ", id_rel, index_b);

    } else {

        //inserisci in testa
        relation **ptr_b = &rel_t_p->relation_receiver_hash[index_b];
        newSingleRel->next_b = *ptr_b;
        *ptr_b = newSingleRel;

        //printf("\n[DEBUG] aggiunta relazione in H_B: %s nell'indice %d che non era vuoto (si collisioni). ", id_rel, index_b);

    }

    //aggiorna i contatori delle entità -> ricevente ce l'ho anche già il puntatore, basta aumentare contatore giusto
    i = rel_t_p->index;
    ent_b->rel_ent_counters[i] = ent_b->rel_ent_counters[i] + 1;
    //printf("\nif(ent_b->rel_ent_counters[%d]: %d == rel_t_p->max: %d)", i, ent_b->rel_ent_counters[i], rel_t_p->max);
    //mette nel relation type l'integer a cui corrisponde nell'indice dell'array delle entità per questo tipo
    //aggiorna il max del tipo di relazione se necessario
    if (ent_b->rel_ent_counters[i] > rel_t_p->max) {

        rel_t_p->max = ent_b->rel_ent_counters[i];
        //cancellare tutte quelle che c'erano prima perchè è un nuovo max
        //relation_t_array[i]->max_inc_rels[i]->
        //inserirlo (in questo caso non serve ordine alfabetico

        //ent_b il nuovo max, cancella tutti quelli precendeti e aggiungi solo questo alla lista dei max

        max_entity* max_ptr = rel_t_p->max_entity_list;
        max_entity* temp;

        if(max_ptr != NULL){
            //elimina la vecchia lista dei max
            while (max_ptr != NULL) {
                temp = max_ptr;

                if(max_ptr->next != NULL) {
                    max_ptr = (max_ptr)->next;
                    free(temp);
                } else {
                    free(temp);
                    break;
                }
            }
        }


        //inserisci il nuovo max in cima alla lista
        max_entity *newMaxEnt;
        newMaxEnt = (max_entity *) malloc(sizeof(max_entity));
        newMaxEnt->next = NULL;
        newMaxEnt->ent_ptr = ent_b;

        newMaxEnt->ent_ptr->rel_ent_counters[i] = rel_t_p->max;

        rel_t_p->max_entity_list = newMaxEnt;



    } else if (ent_b->rel_ent_counters[i] == rel_t_p->max) {
        //aggiorna la lista max per questo tipo di relazione
        //se non c'era nessun altro max inserisci in prima posizione
        if (rel_t_p->max_entity_list == NULL) {

            max_entity *newMaxEnt;
            newMaxEnt = (max_entity *) malloc(sizeof(max_entity));
            newMaxEnt->next = NULL;
            newMaxEnt->ent_ptr = ent_b;
            rel_t_p->max_entity_list = newMaxEnt;

        } else {
            //ent_b è uguale al max, aggiungilo all'array senza cancellare gli altri -> però in ord alfab
            //scorri fino a quando non trovi il posto giusto e inseriscilo
            //printf("\n[DEBUG] devo inserire in ordine alfabetico: %s", ent_b->id_ent);

            max_entity *max_ptr = rel_t_p->max_entity_list;
            max_entity *max_ptr_pre = NULL;


            while (max_ptr != NULL) {

                //printf("\n[DEBUG] contronto %s con %s", max_ptr->ent_ptr->id_ent, ent_b->id_ent);

                if (strcmp(ent_b->id_ent, max_ptr->ent_ptr->id_ent) < 0) {
                    //printf("\nFermati perchè devi inserirlo qui!");
                    break;
                }

                if(max_ptr->next != NULL) {
                    max_ptr_pre = max_ptr;
                    max_ptr = max_ptr->next;
                } else {
                    break;
                }


            }

            //adesso max_ptr punta al primo > e max_ptr_pre punta al primo minore
            //max_ptr_pre->next = quello da aggiungere
            //quello da aggiungere -> next = max_ptr

            max_entity *newMaxEnt;
            newMaxEnt = (max_entity *) malloc(sizeof(max_entity));
            newMaxEnt->ent_ptr = ent_b;

            if(max_ptr_pre != NULL){

                if(strcmp(max_ptr->ent_ptr->id_ent, ent_b->id_ent) > 0){
                    max_ptr_pre->next = newMaxEnt;
                    newMaxEnt->next = max_ptr;
                } else {
                    max_ptr->next = newMaxEnt;
                    newMaxEnt->next = NULL;
                }

            } else {

                if (max_ptr != NULL) {

                    if (strcmp(max_ptr->ent_ptr->id_ent, ent_b->id_ent) > 0) {
                        rel_t_p->max_entity_list = newMaxEnt;
                        newMaxEnt->next = max_ptr;
                    } else {
                        max_ptr->next = newMaxEnt;
                        newMaxEnt->next = NULL;
                    }

                } else {

                    if(rel_t_p->max_entity_list == NULL) {
                        rel_t_p->max_entity_list = newMaxEnt;
                        newMaxEnt->next = NULL;
                    } else {

                        if(strcmp(rel_t_p->max_entity_list->ent_ptr->id_ent, newMaxEnt->ent_ptr->id_ent) > 0){
                            rel_t_p->max_entity_list->next = newMaxEnt;
                            newMaxEnt->next = NULL;

                        } else {
                            rel_t_p->max_entity_list = newMaxEnt;
                            newMaxEnt->next = NULL;
                        }
                    }
                }

            }
        }
    }

        return true;
    }


    bool delRel(char *id_a, char *id_b, char *id_rel) {

        // 1 verifica che ci sia una relazione tra le 2 entità A e B
        // 1.0 verificare che il tipo di relazione esista -> se non esiste fermati
        // 1.1 verificare che relazione esista -> se esiste salvati puntatori a entA e entB
        // 2.1 se non c'è -> non fa nulla
        // 2.2 se c'è -> elimina la relazione; aggiorna contatori entità e lista max
        //          2.2B se era l'ultima relazione rimasta di quel tipo, cancella anche il tipo di relazione

        //printf("\n[DEBUG] chiamato delrel di %s %s %s", id_a, id_b, id_rel);

        relation_t *rel_t_p;
        relation_t *rel_t_p_pre = NULL;

        //se non esistono relazioni sicuramente non esiste la relazione cercata
        if (relation_t_head == NULL)
        {
            return false;

        } else {
            //cerca la relazione di quel tipo
            rel_t_p = relation_t_head;
            bool rel_type_found = false;

            while (!rel_type_found && rel_t_p != NULL)
            {

                if (strcmp(rel_t_p->id_rel, id_rel) == 0)
                {
                    //printf("\n[DEBUG] delRel: il tipo di relazione esiste! Ora cerco se esiste la relazione.");
                    rel_type_found = true;
                    break;
                }

                if (rel_t_p->next != NULL)
                {
                    rel_t_p_pre = rel_t_p;
                    rel_t_p = rel_t_p->next;
                } else
                    break;
            }


            //printf("\n[DEBUG] tipo di relazione di delrel: rel_t_p -> %s, index -> %d, max -> %d", rel_t_p->id_rel,
                   //rel_t_p->index, rel_t_p->max);

            // se il tipo di relazione da eliminare non esiste, non fa nulla
            if (!rel_type_found)
            {
                //printf("\n[DEBUG] rel_type not found!");
                return false;
            }
        }

        //se arrivo qui il tipo di relazione esiste e ho anche il puntatore rel_t_p -> verifico se la relazione esiste o meno
        //se esiste mi salvo puntatore alla relazione, puntatore a ent_A puntatore a ent_B
        //printf("\n[DEBUG] il tipo di relazione esiste, ora verifico se c'è la relazione.");
        int h_a = hash_r(id_a);
        entity *ent_a = NULL;
        relation *rel_p_a = rel_t_p->relation_sender_hash[h_a];
        relation *rel_p_a_pre = NULL;

        //se in questa cella dell'hash table non c'è nessuna relazione sicuramente non c'è
        if (rel_p_a == NULL)
        {
            return false;
        }

        //printf("\n[DEBUG] rel_p_a (testa dell'hash A) -> %s %s %s", rel_p_a->sender->id_ent, rel_p_a->receiving->id_ent, rel_t_p->id_rel);

        //scorri le relazioni in questa cella dell'hash table
        bool rel_found_a = false;
        while (!rel_found_a && rel_p_a != NULL)
        {

            if (strcmp(id_a, rel_p_a->sender->id_ent) == 0 && strcmp(id_b, rel_p_a->receiving->id_ent) == 0)
            {
                rel_found_a = true;
                //printf("\n[DEBUG] relazione trovata in H_A: rel: %s %s %s", rel_p_a->sender->id_ent, rel_p_a->receiving->id_ent, rel_t_p->id_rel);
                break;
            }

            if (rel_p_a->next_a != NULL && !rel_found_a)
            {
                rel_p_a_pre = rel_p_a;
                rel_p_a = rel_p_a->next_a;
            } else
                break;
        }

        //se rel_found è true l'ha trovata, altrimenti non esiste la relazione quindi non fare nulla
        if (!rel_found_a)
        {
            return false;
        }

        /*
        printf("\n[DEBUG] rel_p_a -> %s %s %s", rel_p_a->sender->id_ent, rel_p_a->receiving->id_ent, rel_t_p->id_rel);
        if(rel_p_a_pre == NULL)
            printf("\n[DEBUG] rel_p_a_pre -> NULL");
        else
            printf("\n[DEBUG] rel_p_a_pre -> %s %s %s", rel_p_a_pre->sender->id_ent, rel_p_a_pre->receiving->id_ent, rel_t_p->id_rel);

         */

        //stessa cosa per l'hash table dei receiver
        //se il nome era uguale al precedente il risultato dell'hash è lo stesso non ricalcolarlo
        int h_b;
        if(strcmp(id_a, id_b) == 0)
        {
            h_b = h_a;
        } else
        {
            h_b = hash_r(id_b);
        }
        entity *ent_b = NULL;
        relation *rel_p_b = rel_t_p->relation_receiver_hash[h_b];
        relation *rel_p_b_pre = NULL;

        //printf("\nb-------------------------rel_p_b->%s %s", rel_p_b->sender->id_ent, rel_p_b->receiving->id_ent);

        bool rel_found_b = false;
        while (!rel_found_b && rel_p_b != NULL)
        {
            //printf("\nentro enl while a cercare relpb");

            if (strcmp(id_a, rel_p_b->sender->id_ent) == 0 && strcmp(id_b, rel_p_b->receiving->id_ent) == 0)
            {
                //printf("\n[DEBUG] relazione trovata in H_B: rel: %s %s %s", rel_p_a->sender->id_ent, rel_p_a->receiving->id_ent, rel_t_p->id_rel);
                rel_found_b = true;
                break;
            }

            if (rel_p_b->next_b != NULL && !rel_found_b)
            {
                rel_p_b_pre = rel_p_b;
                rel_p_b = rel_p_b->next_b;
            } else
                break;

        }

        //This should never happen since if the relation is not found in a it will stop
        /*
        if (!rel_found_b)
        {
            //printf("\n[DEBUG] non deve mai succedere perchè c'è in a non può non esserci in b");
        }*/

        //salvati i puntatori a ent_a e ent_b
        ent_a = rel_p_b->sender;
        //printf("\n[DEBUG] ent_a -> %s", ent_a->id_ent);
        ent_b = rel_p_b->receiving;
        //printf("\n[DEBUG] ent_b -> %s", ent_b->id_ent);

        /*
        printf("\n[DEBUG] rel_p_b -> %s %s %s", rel_p_b->sender->id_ent, rel_p_b->receiving->id_ent, rel_t_p->id_rel);
        if(rel_p_b_pre == NULL)
            printf("\n[DEBUG] rel_p_b_pre -> NULL");
        else
            printf("\n[DEBUG] rel_p_b_pre -> %s %s %s", rel_p_b_pre->sender->id_ent, rel_p_b_pre->receiving->id_ent, rel_t_p->id_rel);

        if(rel_p_a_pre != NULL)
            printf("\nDEBUG rel_p_a_pre -> %s %s", rel_p_a_pre->sender->id_ent, rel_p_a_pre->receiving->id_ent);
        if(rel_p_b_pre != NULL)
            printf("\nDEBUG rel_p_b_pre -> %s %s", rel_p_b_pre->sender->id_ent, rel_p_b_pre->receiving->id_ent);
        */
        rem_rel(rel_t_p_pre, rel_t_p, rel_p_a_pre, rel_p_a, rel_p_b_pre, rel_p_b, h_a, h_b);

        return true;
    }

bool delEnt(char *str, entity *e)
{

    //1. cerca l'entità da eliminare cercandola con l'hash
    //2. una volta trovata verifico se nella linked list corrispondente a quell'indice dell'hash table
    //   c'è dentro o no il nome da eliminare. se non c'è non faccio nulla
    //3. se il nome c'è -> 1. elimino tutte le relazioni che hanno a che fare con quel nome
    //                     2. aggiorno i vari contatori / lista di max per il report
    //                     3. elimino l'entità dalla hash table
    int index_ent = hash_n(str);
    bool entityFound = false;

    entity *ptr = entity_hash[index_ent];
    entity *prv = NULL;

    if(ptr == NULL)
    {
        return false;
        //printf("\n nell'index cercato non ho trovato niente");
    }

    while (!entityFound && ptr != NULL)
    {

        if (strcmp(str, ptr->id_ent) == 0)
        {
            entityFound = true;
            //found entity to remove
            //printf("\n[DEBUG] entità da rimuovere trovata!");
            break;

        } else
        {

            if (ptr->next != NULL)
            {
                prv = ptr;
                ptr = ptr->next;
            } else
            {
                break;
            }
        }
    }

    if(!entityFound)
    {
        return false;
    }

    int index_rel = hash_r(str);

    relation_t *rel_t_p = relation_t_head;
    relation_t *rel_t_p_pre = NULL;
    //printf("\nrelation_t_head -> %s", relation_t_head->id_rel);

    relation_t *rel_t_p_tmp = NULL;

    while (rel_t_p != NULL)
    {

        rel_t_p_tmp = rel_t_p->next;

        //scorri la hash table dei sender
        relation *rel_p_a = rel_t_p->relation_sender_hash[index_rel];
        relation *rel_p_a_pre = NULL;

        //scorri tutte lre relazioni di questo indice della hash table
        //if(rel_p_a != NULL)
        //printf("\nrel_p_a->%s %s", rel_p_a->sender->id_ent, rel_p_a->receiving->id_ent);
        int h_a, h_b;
        relation *r_a_pre_next = NULL;
        relation *r_a_next = NULL;



        while (rel_p_a != NULL)
        {

            if (strcmp(rel_p_a->sender->id_ent, str) == 0)
            {
                //printf("\n chiamo delrel per sender!");

                h_a = hash_r(rel_p_a->sender->id_ent);
                h_b = hash_r(rel_p_a->receiving->id_ent);

                relation *rel_p_b_tmp = rel_t_p->relation_receiver_hash[h_b];
                relation *rel_p_b_tmp_pre = NULL;

                while (rel_p_b_tmp != NULL) {


                    if (strcmp(rel_p_a->receiving->id_ent, rel_p_b_tmp->receiving->id_ent) == 0 &&
                        strcmp(rel_p_a->sender->id_ent, rel_p_b_tmp->sender->id_ent) == 0)
                    {
                        break;
                    }

                    if (rel_p_b_tmp->next_b != NULL)
                    {
                        rel_p_b_tmp_pre = rel_p_b_tmp;
                        rel_p_b_tmp = rel_p_b_tmp->next_b;
                    } else
                    {
                        break;
                    }
                }

                rem_rel_ret = rem_rel(rel_t_p_pre, rel_t_p, rel_p_a_pre, rel_p_a, rel_p_b_tmp_pre, rel_p_b_tmp, h_a, h_b);

                //nel caso in cui cancello la relazione perdo i puntatori a next dentro la rel, quindi devo salvarmeli prima

                if (rem_rel_ret.p_a_next != NULL)
                {
                    rel_p_a_pre = rem_rel_ret.p_a_pre;
                    rel_p_a = rem_rel_ret.p_a_next;
                } else
                {
                    break;
                }

                } else
                {

                    //nel caso in cui non cancello la relazione posso andare avanti alla vecchia maniera
                    if (rel_p_a->next_a != NULL)
                    {
                        rel_p_a_pre = rel_p_a;
                        rel_p_a = rel_p_a->next_a;
                    } else
                    {
                        break;
                    }
                }
            }

            //scorri la hash table dei receiver
            relation *rel_p_b = rel_t_p->relation_receiver_hash[index_rel];
            relation *rel_p_b_pre = NULL;

            relation *r_b_pre_next = NULL;
            relation *r_b_next = NULL;

            while (rel_p_b != NULL)
            {

                if (strcmp(rel_p_b->receiving->id_ent, str) == 0)
                {
                    //printf("\n chiamo delrel per receiver!");
                    h_a = hash_r(rel_p_b->sender->id_ent);
                    h_b = hash_r(rel_p_b->receiving->id_ent);

                    relation *rel_p_a_tmp = rel_t_p->relation_sender_hash[h_a];
                    relation *rel_p_a_tmp_pre = NULL;

                    while (rel_p_a_tmp != NULL) {

                        if (strcmp(rel_p_b->receiving->id_ent, rel_p_a_tmp->receiving->id_ent) == 0 &&
                            strcmp(rel_p_b->sender->id_ent, rel_p_a_tmp->sender->id_ent) == 0)
                        {

                            break;
                        }

                        if (rel_p_a_tmp->next_a != NULL)
                        {
                            rel_p_a_tmp_pre = rel_p_a_tmp;
                            rel_p_a_tmp = rel_p_a_tmp->next_a;
                        } else
                        {
                            break;
                        }
                    }

                        //printf("\nHASH B CANCELLA rel(%s, %s)", rel_p_b->sender->id_ent,rel_p_b->receiving->id_ent);
                        rem_rel_ret = rem_rel(rel_t_p_pre, rel_t_p, rel_p_a_tmp_pre, rel_p_a_tmp, rel_p_b_pre, rel_p_b, h_a, h_b);

                        if (rem_rel_ret.p_b_next != NULL)
                        {
                            rel_p_b_pre = rem_rel_ret.p_b_pre;
                            rel_p_b = rem_rel_ret.p_b_next;
                        } else
                        {
                            break;
                        }

                    } else
                    {

                        if (rel_p_b->next_b != NULL)
                        {
                            rel_p_b_pre = rel_p_b;
                            rel_p_b = rel_p_b->next_b;
                        } else
                        {
                            break;
                        }
                    }
                }

            if (rel_t_p_tmp != NULL)
            {
                rel_t_p_pre = rel_t_p;
                rel_t_p = rel_t_p_tmp;
            } else
            {
                break;
            }

            //rem_rel_ret.deletedRelType = false;

        }


        //RIMUOVI ENTITA DALLA HASH TABLE DELLE ENTITA
        //3 casi
        //A) l'entità da eliminare è il primo della lista
        //B) l'entità da eliminare è in mezzo alla lista
        //c) l'entità da eliminare è in fondo alla lista
        /*if (!entityFound) {
            return false;
        }*/

        /*
        printf("ptr-> %s", ptr->id_ent);
        if(ptr->next != NULL)
            printf("ptr->next %s", ptr->next->id_ent);
        else
            printf("ptr->next null");
        if(prv != NULL)
            printf("prv -> %s", prv->id_ent);
        else
            printf("prv -> null");
        //printf("\nentità da eliminare a cui sto puntando: %s", ptr->id_ent);*/

        if (ptr->next == NULL && prv == NULL)
        {
            entity_hash[index_ent] = NULL;
            free(ptr);
        } else if (prv == NULL && ptr->next != NULL)
        {
            entity_hash[index_ent] = ptr->next;
            free(ptr);
        } else if (ptr->next != NULL && prv != NULL)
        {
            prv->next = ptr->next;
            free(ptr);
        } else if (prv != NULL && ptr->next == NULL)
        {
            prv->next = NULL;
            free(ptr);
        }

    //se non trova entità da eliminare non fa nulla
    //printf("\n[DEBUG] Entità da rimuovere non trovata!");
    return true;

    }

    rem_rel_t rem_rel(relation_t *rel_t_p_pre, relation_t *rel_t_p,
            relation *rel_p_a_pre, relation *rel_p_a, relation *rel_p_b_pre, relation *rel_p_b,
            int h_a, int h_b) {

        rem_rel_ret.p_a_pre = NULL;
        rem_rel_ret.p_b_pre = NULL;

        rem_rel_ret.p_a_next = NULL;
        rem_rel_ret.p_b_next = NULL;
        //rem_rel_ret.deletedRelType = false;

        //printf("\nrem_rel(%s,%s), rel_t_p -> %s\n", rel_p_a->sender->id_ent, rel_p_a->receiving->id_ent, rel_t_p->id_rel);
        /*
        if(rel_p_a_pre != NULL && rel_p_b_pre != NULL)
            printf("\n[DEBUG] rem_rel(%s, A pre -> (%s,%s), A (%s,%s), B pre -> (%s,%s), B -> (%s,%s)",
                    rel_t_p->id_rel, rel_p_a_pre->receiving->id_ent, rel_p_a_pre->sender->id_ent, rel_p_a_pre->receiving->id_ent,
                    rel_p_a->receiving->id_ent, rel_p_a->sender->id_ent, rel_p_b_pre->receiving->id_ent, rel_p_b_pre->sender->id_ent,
                    rel_p_b->receiving->id_ent, rel_p_b->sender->id_ent);
        else if(rel_p_a_pre == NULL && rel_p_b_pre != NULL)
            printf("\n[DEBUG] rem_rel(%s, A pre -> (NULL), A (%s,%s), B pre -> (%s,%s), B -> (%s,%s)",
                   rel_t_p->id_rel, rel_p_a->receiving->id_ent, rel_p_a->sender->id_ent,
                   rel_p_b_pre->receiving->id_ent, rel_p_b_pre->sender->id_ent,
                   rel_p_b->receiving->id_ent, rel_p_b->sender->id_ent);
        else if(rel_p_a_pre != NULL && rel_p_b_pre == NULL)
            printf("\n[DEBUG] rem_rel(%s, A pre -> (%s,%s), A (%s,%s), B pre -> (NULL), B -> (%s,%s)",
                   rel_t_p->id_rel, rel_p_a_pre->receiving->id_ent, rel_p_a_pre->sender->id_ent, rel_p_a_pre->receiving->id_ent,
                   rel_p_a->receiving->id_ent, rel_p_a->sender->id_ent,
                   rel_p_b->receiving->id_ent, rel_p_b->sender->id_ent);
        else if(rel_p_a_pre == NULL && rel_p_b_pre == NULL)
            printf("\n[DEBUG] rem_rel(%s, A pre -> (NULL), A (%s,%s), B pre -> (NULL), B -> (%s,%s)",
                   rel_t_p->id_rel,
                   rel_p_a->receiving->id_ent, rel_p_a->sender->id_ent,
                   rel_p_b->receiving->id_ent, rel_p_b->sender->id_ent);*/


        //adesso ho tutto l'occorrente per cancellare la relazione
        entity *ent_b = rel_p_a->receiving;

        //printf("\n[DEBUG] ent_b -> %s", ent_b->id_ent);
        /*if(rel_t_p_pre != NULL)
            printf("\n[DEBUG] rel_t_p_pre -> %s, index %d", rel_t_p_pre->id_rel, rel_t_p_pre->max);
        else
            printf("\n[DEBUG] rel_t_p_pre -> NULL");

        printf("\n[DEBUG] rel_t_p -> %s, index: %d", rel_t_p->id_rel, rel_t_p->index);*/

        //diminuisci il contatore del numero di relazioni entranti per ent_b
        //se ent_b era nella lista delle entità dei max
        //verifica + fast -> prima verifico se il suo contatore prima di diminuirlo è uguale al max di quel tipo
        //se non lo è concludo subito che non era nella lista dei max])
        //se c'è solo lui -> rimuovilo, diminuisci il max per quel tipo di relazione, trova il/i nuovi max
        //(devo scorrere tutte le entità riceventi di questo tipo di relazione per ricostruire la lista dei max
        //se ent_b non era nella lista dei max
        //rimuovilo e basta
        //printf("\n[DEBUG] contatore di %s della rel %s prima di rimuoverlo: %d, index: %d", ent_b->id_ent, rel_t_p->id_rel, ent_b->rel_ent_counters[rel_t_p->index], rel_t_p->index);
        int prev_ent_b_cont = ent_b->rel_ent_counters[rel_t_p->index];

        ent_b->rel_ent_counters[rel_t_p->index] = ent_b->rel_ent_counters[rel_t_p->index] - 1;
        //printf("\n[DEBUG] aggiorno contatore di entb->%s tipo entita %s index %d dal vecchio valore %d al nuovo %d", ent_b->id_ent, rel_t_p->id_rel, rel_t_p->index, prev_ent_b_cont,  ent_b->rel_ent_counters[rel_t_p->index]);


        //printf("\n[DEBUG] contatore di %s della rel %s dopo: %d", ent_b->id_ent, rel_t_p->id_rel, ent_b->rel_ent_counters[rel_t_p->index]);

        bool remRelType = false;
        max_entity *max_ptr = rel_t_p->max_entity_list;

        //se la relazione da rimuovere contribuiva a rendere ent_b uno dei max riceventi
        if (prev_ent_b_cont == rel_t_p->max)
        {

            //dovrei farlo solo quando c'era solo lui tra i max il ricalcolo
            if(rel_t_p->max_entity_list->next == NULL)
            {
                //SE trovi ancora dei nuovi max -> elimina vecchia lista dei max, fai quella nuova
                //SE non trovi dei nuovi max -> elimina completamente anche il tipo di relazione

                //1.0 cancella la vecchia lista dei max -> va freeate in ogni caso
                //printf("\nrel_t_p -> %s", rel_t_p->id_rel);

                //max_ptr = rel_t_p->max_entity_list;
                max_entity *tmp_next = NULL;
                while (max_ptr != NULL)
                {
                    //printf("\nmax_ptr -> %s", max_ptr->ent_ptr->id_ent);
                    tmp_next = max_ptr->next;

                    if (max_ptr != rel_t_p->max_entity_list)
                    {
                        free(max_ptr);
                    }

                    if (tmp_next != NULL)
                        max_ptr = tmp_next;
                    else
                       break;
                }

                free(rel_t_p->max_entity_list);
                rel_t_p->max_entity_list = NULL;
                max_ptr = NULL;
                /*
                printf("\n[DEBUG] adesso lista dei max deve esere vuota!");
                if(rel_t_p->max_entity_list != NULL)
                    printf("\ninvece non è vuota: c'è %s", rel_t_p->max_entity_list->ent_ptr->id_ent);
                else
                    printf("\nva beene");*/


                //1.1 scorro tutti i riceventi della relazione rel_t_p (rel_t_p->relation_receiver_hash)
                //1.2 aggiungi tutte le entità con max == nuovo max alla nuova lista (quando trovi la prima setta a true bool)
                bool newMaxFound = false;
                relation *rel_p = NULL;
                //printf("\nTrova il nuovo MAX!");
                int newMax = 0;

                for (int k = 0; k < DEF_REL_N; k++)
                {
                    if (rel_t_p->relation_receiver_hash[k] != NULL)
                    {
                        rel_p = rel_t_p->relation_receiver_hash[k];

                        while (rel_p != NULL)
                        {

                            if (rel_p->receiving->rel_ent_counters[rel_t_p->index] > newMax)
                            {
                                newMax = rel_p->receiving->rel_ent_counters[rel_t_p->index];
                            }

                            if (rel_p->next_b != NULL)
                                rel_p = rel_p->next_b;
                            else
                                break;
                        }
                    }
                }

                //printf("\n1 tipo di relazione di delrel rel_t_p->%s", rel_t_p->id_rel);
                //printf("\nnewmax: %d\n", newMax);


                if (newMax == 0)
                {
                    rel_t_p->max = 0;
                    ent_b->rel_ent_counters[rel_t_p->index] = 0;
                    //printf("\nDEBUG nuovo max non trovato! -> elimina il tipo di relazione");
                    newMaxFound = false;
                } else
                {
                    rel_t_p->max = newMax;
                    for (int k = 0; k < DEF_REL_N; k++)
                    {
                        if (rel_t_p->relation_receiver_hash[k] != NULL)
                        {
                            rel_p = rel_t_p->relation_receiver_hash[k];


                            while (rel_p != NULL)
                            {

                                bool alreadyAdded = false;

                                //ignore rel to be deleted since it will be deleted at the end of the function
                                if (rel_p->receiving->rel_ent_counters[rel_t_p->index] == newMax)
                                {

                                    newMaxFound = true;
                                    //printf("\n[DEBUG] Ho trovato un nuovo max: %s, nella rel_hash_rec[%d]", rel_p->receiving->id_ent, k);

                                    //inserisci in ordine alfabetico alla lista dei max
                                    max_ptr = rel_t_p->max_entity_list;
                                    max_entity *max_ptr_pre = NULL;


                                    //adesso max_ptr punta al primo > e max_ptr_pre punta al primo minore
                                    //max_ptr_pre->next = quello da aggiungere
                                    //quello da aggiungere -> next = max_ptr

                                    while (max_ptr != NULL)
                                    {

                                        //printf("\n[DEBUG] contronto %s con %s", max_ptr->ent_ptr->id_ent, rel_p_b->receiving->id_ent);
                                               //ent_b->id_ent);
                                        if (strcmp(rel_p->receiving->id_ent,max_ptr->ent_ptr->id_ent) < 0)
                                        {
                                            break;
                                        } else if (strcmp(max_ptr->ent_ptr->id_ent, rel_p->receiving->id_ent) == 0)
                                        {
                                            //rel_p->receiving->id_ent) == 0){
                                            alreadyAdded = true;
                                            break;
                                        }

                                        if(max_ptr->next != NULL)
                                        {
                                            max_ptr_pre = max_ptr;
                                            max_ptr = max_ptr->next;
                                        } else
                                        {
                                            break;
                                        }

                                    }



                                    //adesso max_ptr punta al primo > e max_ptr_pre punta al primo minore
                                    //max_ptr_pre->next = quello da aggiungere
                                    //quello da aggiungere -> next = max_ptr
                                    if (!alreadyAdded)
                                    {
                                        max_entity *newMaxEnt;
                                        newMaxEnt = (max_entity *) malloc(sizeof(max_entity));

                                        newMaxEnt->ent_ptr = rel_p->receiving;



                                        if(max_ptr_pre != NULL)
                                        {

                                            if(strcmp(max_ptr->ent_ptr->id_ent, newMaxEnt->ent_ptr->id_ent) > 0)
                                            {
                                                max_ptr_pre->next = newMaxEnt;
                                                newMaxEnt->next = max_ptr;
                                            } else
                                            {
                                                max_ptr->next = newMaxEnt;
                                                newMaxEnt->next = NULL;
                                            }

                                        } else
                                        {

                                            if (max_ptr != NULL)
                                            {

                                                if (strcmp(max_ptr->ent_ptr->id_ent, newMaxEnt->ent_ptr->id_ent) >
                                                    0)
                                                {
                                                    rel_t_p->max_entity_list = newMaxEnt;
                                                    newMaxEnt->next = max_ptr;
                                                } else
                                                {
                                                    max_ptr->next = newMaxEnt;
                                                    newMaxEnt->next = NULL;
                                                }

                                            } else
                                            {

                                                rel_t_p->max_entity_list = newMaxEnt;
                                                newMaxEnt->next = NULL;

                                            }
                                        }
                                    }
                                }

                                if (rel_p->next_b != NULL)
                                {
                                    rel_p = rel_p->next_b;
                                }
                                else
                                    break;

                            }
                        }

                    }
                }


                //1.3 se alla fine newMaxFound è false -> cancella il tipo di relazione
                if (!newMaxFound)
                {
                    //printf("\n[DEBUG] Cancella anche il tipo di relazione!");
                    remRelType = true;
                }

            } else
            {

                //se ci sono anche altri nella lista dei max rimuovo solo lui dalla lista dei max e fine
                //2 casi -> devo toglierlo dalla cima o toglierlo generico
                //se come prima ci sono 2 casi del caso A
                //printf("\nCASO B: RIMUOVI SOLO LUI DALLA LISTA DEI MAX");
                //max_entity *max_ptr = rel_t_p->max_entity_list;
                max_entity *max_ptr_pre = NULL;

                bool found = false;
                while (!found && max_ptr != NULL)
                {
                    //printf("\nstrcmp (%s, %s)", max_ptr->ent_ptr->id_ent, ent_b->id_ent);
                    if (strcmp(max_ptr->ent_ptr->id_ent, ent_b->id_ent) == 0)
                    {
                        //printf("\ntrovato quello da rimuovere dalla lista dei max: %s", ent_b->id_ent);
                        found = true;
                        break;
                    }

                    if (max_ptr->next != NULL)
                    {
                        max_ptr_pre = max_ptr;
                        max_ptr = max_ptr->next;
                    } else break;
                }

                //printf("\nquello da togliere dai max per %s è %s",rel_t_p->id_rel, max_ptr->ent_ptr->id_ent);

                    //adesso se pre è null vuol dire che quello cercato è la testa
                    //se pre è != NULL allora devo togliere quello dopo la testa
                    if (max_ptr_pre == NULL && max_ptr->next == NULL)
                    {
                        rel_t_p->max_entity_list->ent_ptr = max_ptr->next->ent_ptr;
                        rel_t_p->max_entity_list->next = NULL;
                        free(max_ptr);
                    } else if (max_ptr_pre != NULL && max_ptr->next == NULL)
                    {
                        max_ptr_pre->next = NULL;
                        free(max_ptr);
                    } else if (max_ptr_pre != NULL && max_ptr->next != NULL)
                    {
                        max_ptr_pre->next = max_ptr->next;
                        free(max_ptr);
                    } else if (max_ptr_pre == NULL && max_ptr->next != NULL)
                    {
                        //printf("\n[DEGUG] DIO CAN CASO D non deve mai succedere, errore!");
                        rel_t_p->max_entity_list = max_ptr->next;
                        free(max_ptr);
                    }
            }
        }


        //printf("\nrel_p_a -> %s %s", rel_p_a->receiving->id_ent, rel_p_a->sender->id_ent);
        //printf("\nrel_p_b -> %s %s", rel_p_b->receiving->id_ent, rel_p_b->sender->id_ent);

        //ent_b non era nella lista dei max, rimuovilo e basta
        //a questo punto posso rimuovere la relazione dalle strutture dati
        //rimuovilo da entrambe le hash table!
        //CASO A c'è solo la relazione cercata nella hash table
        //CASO B ci sono anche altre relazioni nella hasha table (collisioni)

        rem_rel_ret.p_a_next = rel_p_a->next_a;
        rem_rel_ret.p_b_next = rel_p_b->next_b;

        rem_rel_ret.p_a_pre = rel_p_a_pre;
        rem_rel_ret.p_b_pre = rel_p_b_pre;


        //CASO A
        //elimino da hash table sender (a)
        //se rel_p_a_pre == NULL && rel_p_a->next == NULL -> c'era solo una relazione nella cella della hash table
        if (rel_p_a_pre == NULL && rel_p_a->next_a == NULL) {
            //free(rel_p_a);
            rel_t_p->relation_sender_hash[h_a] = NULL;
            //rel_p_a = NULL;
        } else if (rel_p_a_pre == NULL && rel_p_a->next_a != NULL) {

            //ce ne sono 2 -> dipende se sono prima o dopo
            if (rel_p_a == rel_t_p->relation_sender_hash[h_a]) {
                //prima
                rel_t_p->relation_sender_hash[h_a] = rel_p_a->next_a;
                //free(rel_p_a);
            } else {
                //dopo
                rel_p_a_pre->next_a = rel_p_a->next_a;
            }
        } else if (rel_p_a->next_a != NULL && rel_p_a_pre != NULL) {
            //generico
            rel_p_a_pre->next_a = rel_p_a->next_a;
        } else if(rel_p_a->next_a == NULL && rel_p_a_pre != NULL){
            //printf("\n\n--------------------------è qui l'errore?");
            rel_p_a_pre->next_a = NULL;
        } else {
            //printf("\nNON DEVE MAI SUCCEDERE!!");
        }

        //CASO B
        //elimino da hash table receiver (b)
        //se rel_p_b_pre == NULL && rel_p_b->next_b == NULL -> c'era solo una relazione nella cella della hash table
        /*
        if(rel_p_b != NULL)
            printf("\n[DEBUG] rel_p_b -> %s %s %s", rel_p_b->receiving->id_ent, rel_p_b->sender->id_ent, rel_t_p->id_rel);
        if(rel_p_b_pre != NULL)
            printf("\n[DEBUG] rel_p_b_pre -> %s %s %s", rel_p_b_pre->receiving->id_ent, rel_p_b_pre->sender->id_ent, rel_t_p->id_rel);
        else
            printf("\n[DEBUG] rel_p_b_pre -> NULL");
        */

        if (rel_p_b_pre == NULL && rel_p_b->next_b == NULL) {
            free(rel_p_b);
            rel_t_p->relation_receiver_hash[h_b] = NULL;
            //rel_p_b = NULL;
        } else if (rel_p_b_pre == NULL && rel_p_b->next_b != NULL) {
            //ce ne sono 2 -> dipende se sono prima o dopo
            if (rel_p_b == rel_t_p->relation_receiver_hash[h_b]) {
                //prima
                rel_t_p->relation_receiver_hash[h_b] = rel_p_b->next_b;
                free(rel_p_b);
                //rel_p_b = NULL;
            } else {
                //dopo
                rel_p_b_pre->next_b = rel_p_b->next_b;
                free(rel_p_b);
                //rel_p_b = NULL;
            }
        } else if (rel_p_b->next_b != NULL && rel_p_b_pre != NULL) {
            //generico
            rel_p_b_pre->next_b = rel_p_b->next_b;
            free(rel_p_b);
            //rel_p_b = NULL;
        } else if(rel_p_b->next_b == NULL && rel_p_b_pre != NULL){
            //rel_t_p->relation_receiver_hash[h_b] = NULL;
            rel_p_b_pre->next_b = NULL;
            free(rel_p_b);
            //rel_p_b = NULL;
        } else {
            //printf("\nNON DEVE MAI SUCCEDERE");
        }
        return rem_rel_ret;
    }

    void report()
    {
        //format: esempi
        // "amico_di" "bruno" 2; "compagno_di" "dario" 1;
        // "amico_di" "bruno" 2; "compagno_di" "alice" "dario" 1;

        relation_t *rel_t_ptr = relation_t_head;
        bool none = true;
        bool space = false;

        while (rel_t_ptr != NULL)
        {

            char output[DEF_OUTPUT_L] = "\0";

            if(rel_t_ptr->max > 0)
            {
                none = false;
                if (space)
                {
                    strcat(output, " ");
                }

                strcat(output, virgoletta);
                strcat(output, rel_t_ptr->id_rel);
                strcat(output, virgolettaspazio);

                max_entity *ptr = rel_t_ptr->max_entity_list;
                //printf("\nrel_t_ptr->max_entity_list-> %s\n", ptr->ent_ptr->id_ent);
                while (ptr != NULL)
                {
                    strcat(output, virgoletta);
                    strcat(output, ptr->ent_ptr->id_ent);
                    strcat(output, virgolettaspazio);
                    ptr = ptr->next;
                }

                fputs(output, stdout);
                printf("%d;", rel_t_ptr->max);
                space = true;
            }

            rel_t_ptr = rel_t_ptr->next;

        }

        if(none)
            fputs("none\n", stdout);
        else
            fputs("\n", stdout);

    }

    void print() {


        printf("[DEBUG] Lista entità: ");
        entity *tmp;

        for (int i = 0; i < DEF_ENT_N; ++i) {

            if(entity_hash[i] != NULL) {
                tmp = entity_hash[i];

                while (tmp->next != NULL) {
                    if (tmp->id_ent != NULL) {
                        printf("\n %s ", tmp->id_ent);
                        for (int j = 0; j < DEF_MAX_REL_L; ++j) {
                            printf("\nent_b_counters[%d] = %d", j, tmp->rel_ent_counters[j]);
                        }
                        printf("\n-------------------------------------------");
                        tmp = tmp->next;
                    }

                }

                printf("\n %s "
                       "", tmp->id_ent);
                for (int j = 0; j < DEF_MAX_REL_L; ++j) {
                    printf("\nent_b_counters[%d] = %d", j, tmp->rel_ent_counters[j]);
                }
                printf("\n----------------------------------------");
            }
        }

        printf("\n[DEBUG] Stato attuale delle liste di relazioni: \n");
        relation_t *rel_t_p = relation_t_head;
        while (rel_t_p != NULL) {
            printf("\n[DEBUG] TIPO DI RELAZIONE: %s", rel_t_p->id_rel);
            printf("\n[DEBUG] MAX = %d", rel_t_p->max);
            for (int k = 0; k < DEF_REL_N; ++k) {
                    //scorri k-esima posizione della hash table

                    if (rel_t_p->relation_sender_hash[k] != NULL) {

                        relation *ptr = rel_t_p->relation_sender_hash[k];
                        while (ptr != NULL) {
                            printf("\nhash_table_sender[%d]: ", k);
                            printf("\nid_datore: %s", ptr->sender->id_ent);
                            printf("\nid_ricevente: %s", ptr->receiving->id_ent);
                            ptr = ptr->next_a;
                        }
                    }

                    if (rel_t_p->relation_receiver_hash[k] != NULL) {

                        relation *ptr = rel_t_p->relation_receiver_hash[k];
                        while (ptr != NULL) {
                            printf("\nhash_table_receiver[%d]: ", k);
                            printf("\nid_datore: %s", ptr->sender->id_ent);
                            printf("\nid_ricevente: %s", ptr->receiving->id_ent);
                            ptr = ptr->next_b;
                        }
                    }

                }

            rel_t_p = rel_t_p->next;

        }

        relation_t *rel_t_ptr = relation_t_head;
        printf("\n[DEBUG] STATO DELLA LISTA DI TIPI DI RELAZIONI: ");
        while(rel_t_ptr != NULL)
        {
            printf("\nid_rel-> %s", rel_t_ptr->id_rel);
            printf("\tmax-> %d", rel_t_ptr->max);
            printf("\tindex: %d", rel_t_ptr->index);

            if(rel_t_ptr->next != NULL)
                rel_t_ptr = rel_t_ptr->next;
            else
                break;

        }
}


