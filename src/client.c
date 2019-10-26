#include "../include/client/support.h"
// Il main
int main(int argc, char *argv[]){
    // Controllo che il numero di argomenti
    // Sia coerente
    ASSERT(argc == 3 , "Argomenti");
    // Il mio valore p da linea di comando
    uint32_t p = atoi(argv[1]);
    // Il mio valore k da linea di comando
    uint32_t k = atoi(argv[2]);
    // Il mio valore w da linea di comando
    uint32_t w = atoi(argv[3]);
    // Guardo se soddisfano le condizioni 
    ASSERT(p < 1 || p > k || w <= 3*p,"Argomenti passati non validi ! \n");
    // Variabili di supporto
    struct timespec t1;
    clock_gettime(CLOCK_REALTIME, &t1);
    srand((getpid() * t1.tv_sec) ^ t1.tv_nsec);
    // Genero il mio secret
    uint16_t secret = generate_secret();
    // Genero il mio id 
    uint64_t id = generate_id();
    // Stampo il mio messagigo di inizio
    printf("CLIENT %"PRIx64"", id);
    printf(" SECRET %"PRIu16"\n", secret);
    fflush(stdout);
    // Questo array contiene un set 
    // con i valori dei server a cui connettermi 
    server* s_array = ALLOC(sizeof(server) * p , server*);
    ASSERT(s_array == NULL, "MALLOC");
    // Popolo il mio array
    populate_server_array(s_array,p,k);
    // Il mio messsaggio in nbo contenente l'id
    uint64_t nbo = generate_nbo(id);
    // E' tempo di dormire
    struct timespec t = { (secret / 1000) , (secret % 1000) * 1e6 };
    // Mi connetto w volte al gruppo di server in s_array
    sender(nbo,w,k,p,s_array,t);
    // Chiudo tutti i descrittori
    close_des(s_array,p);
    //Stampo il messaggio di end
    printf("CLIENT %"PRIx64" DONE\n", id);
    fflush(stdout);
    // Libero la memoria in cui sono 
    // memorizzati i server
    free(s_array);
    // Restituisco zero
    return 0;
}