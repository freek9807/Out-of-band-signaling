#include"support.h"
// Guardo che non ci siano valori di p duplicati
int is_in_array(server* s_array,int start,int end,uint32_t value){
    for(int i = start; i < end; i++)
        if(value == s_array[i].value)
            return 1;
    
    return 0;
}
// Dimensione di un intero castato in char
int dim_int_in_char(uint32_t value){
    int count = 0;
    while(value != 0){
        value /= 10;
        ++count;
    }
    return 
         count * sizeof(char);
}
// restituisco la dimensione del nome di un server
int size_of_server_name(uint32_t to_server){
    return
        sizeof(SOCKET) + sizeof(EXT) + dim_int_in_char(to_server) + 1;
}
// Genero un nuovo secret
uint16_t generate_secret(){
    return (rand() % 3000) +1;
}
// Genero un nuovo id
uint64_t generate_id(){
    return
        (uint64_t) rand() << 32 | rand();
}
// Popolo l'array che contiene i server a cui connettersi 
void populate_server_array(server* s_array,uint32_t p,uint32_t k){
    for(int i = 0; i < p; i++)
        do{
            s_array[i].value = (rand() % k) + 1;
            s_array[i].active = 0;
        }while(is_in_array(s_array,0,i,s_array[i].value)); 
}
// Genero il valore in nbo
uint64_t generate_nbo(uint64_t id){
    uint32_t high_part = htonl(id >> 32);
    uint32_t low_part = htonl((id << 32) >> 32);
    return
         ((uint64_t) low_part << 32) | high_part;
}
// Chiudo tutti i descrittori 
void close_des(server* s_array,uint32_t p){
    for(int i = 0; i < p; i++)
        close(s_array[i].descriptor);
}
// Invia il messaggio al server
void sender(uint64_t nbo,uint32_t w,uint32_t k,uint32_t p,server* s_array,struct timespec t){   
    // La stringa che contiene il nome del server
    char *s1;
    // Indirizzo del socket
    struct sockaddr_un sa; 
    sa.sun_family=AF_UNIX;    
    for(int i = 0; i < w ; i++){
        // Il server a cui connettermi
        uint32_t to_server = (rand() % p);
        // La dimensione del nome del mio server
        s1 = ALLOC(size_of_server_name(s_array[to_server].value) , char*);
        ASSERT(s1 == NULL, "MALLOC");
        // Il nome della mia stringa
        sprintf(s1, SOCKET "%d" EXT, s_array[to_server].value);
        // Copio l'indirizzo a cui connettermi
        strncpy(
            sa.sun_path,
            s1,
            UNIX_PATH_MAX
        );
        // Se non c'è una connessione già attiva
        if(!s_array[to_server].active){
            // Imposto il descrittore della socket
            s_array[to_server].descriptor = socket(AF_UNIX,SOCK_STREAM,0);
            // Se non riesco a connettermi
            if (connect(s_array[to_server].descriptor, (struct sockaddr *)&sa, sizeof(sa)) == -1){
                printf("Errore di connessione %s\n", s1);
                exit(-1);
            }
                /*   ASSERTOK(
                    connect(s_array[to_server].descriptor, (struct sockaddr *)&sa, sizeof(sa)),
                    "Connection error !");   */
                // La connessione è stabilita
                s_array[to_server].active = 1;
        }
        // Invio il messaggio
        send(s_array[to_server].descriptor, &nbo, sizeof(uint64_t), 0);
        // Dormo SECRET ms
        nanosleep(&t,NULL);
        // Libero la memoria della stringa 
        free(s1);
    }
}