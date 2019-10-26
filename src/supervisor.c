#include "../include/supervisor/support.h"
// Variabili di supporto
volatile sig_atomic_t stop = 0;
volatile sig_atomic_t print = 0;
msg_list *list[HASH_DIM];
time_t time_last = 0;
// Array di mutex che si riferiscono
// agli elementi di list
pthread_mutex_t lock[HASH_DIM];
// Inizializzo list e lock
void set_hash_table()
{
    for (int i = 0; i < HASH_DIM; i++)
    {
        list[i] = NULL;
        int result = pthread_mutex_init(&lock[i], NULL);
        ASSERT(result != 0, "Mutex");
    }
}
// Inizio la stampa dell'hash table
void *print_ls(void *args)
{
    print_hash_table(stderr, list, HASH_DIM);
    return 0;
}
// Signal handler di SIGINT 
void sigint(int signo)
{
    // Prendo il tempo
    time_t time_now = time(NULL);
    //Guardo se è passato un minuto
    if (time_now - time_last <= 1)
    {
        stop = 1;
        return;
    }
    // Aggiorno il tempo
    time_last = time_now;
    print = 1;
}
// Guardo se inserire o aggiornare i valori nella hash table
void *new_data_hash_table(void *val)
{
    message_t *msg = (message_t *)val;
    // Guardo il valore di hash
    int hash = calculate_hash_vl(msg->id, HASH_DIM);
    // Faccio la lock di quell'indice
    pthread_mutex_lock(&lock[hash]);
    // Guardo se inserire nuovo o aggiornare
    if (list[hash] == NULL)
        list[hash] = add_hash_table(*msg, NULL, 1);
    else
        list[hash] = search_hash_table(*msg, list[hash]);
    // Faccio la unlock dell'indice
    pthread_mutex_unlock(&lock[hash]);
    return NULL;
}
// Stampo la hash table
void print_hash_table(FILE *file, msg_list **ls, int dim)
{
    fflush(file);
    for (int i = 0; i < dim; i++)
    {
        // Faccio la lock
        pthread_mutex_lock(&lock[i]);
        print_list(file, ls[i]);
        // Faccio una unlock
        pthread_mutex_unlock(&lock[i]);
    }
}
// Crea un thread per la stampa
threads *handle_print(threads *list_threads)
{
    pthread_t thread1;
    // Nuovo Thread
    int res = pthread_create(&thread1, NULL, print_ls, NULL);
    ASSERT(res != 0, "Thread");
    // Restituisce la lista con i thread aggiornata
    return add_thread(thread1, list_threads);
}

int main(int argc, char *argv[])
{
    // Variabili di supporto del main
    struct sigaction s;
    ASSERT(argc == 1, "ARGS");
    int k = atoi(argv[1]);
    ASSERT(k == 0,"ATOI");
    pid_t *pids = malloc(sizeof(pid_t) * k);
    ASSERT(pids == NULL, "MALLOC");
    int **pipes = malloc(sizeof(int *) * k);
    ASSERT(pids == NULL, "MALLOC");
    char **file_names = malloc(sizeof(char *) * k);
    ASSERT(pids == NULL, "MALLOC");
    char **i_to_s = malloc(sizeof(char *) * k);
    ASSERT(pids == NULL, "MALLOC");
    threads *list_threads = NULL;
    fd_set readset, set;
    sigset_t sigset, oldset;
    // Inizio dell'esecuzione del supervisor
    fprintf(stdout, "SUPERVISOR STARTING %d \n", k);
    fflush(stdout);
    // Setto l'handler
    set_signal_handler(&s, &sigset, &oldset);
    // Setto la hash table
    set_hash_table();
    // Setto gli array appena allocati
    set_array(pipes, file_names, i_to_s, k);
    // Genero i  server
    for (int i = 0; i < k; i++){
        pids[i] = generate_server(i, pipes, file_names, i_to_s);
        // Chiudo la scrittura
        close(pipes[i][1]);
    }
    // Trovo il max dei descrittori della select
    int max = set_pselect_desc(k,&set,pipes);
    while (!stop)
    {
        // Setto i descrittori
        readset = set;
        // Inizio ad ascoltare sui fd
        int res = pselect(max + 1, &readset, NULL, NULL, NULL, &oldset);
        ASSERT(res < 0 && errno != EINTR, "Select");
        // Se la print è settata 
        if (print == 1)
        {
            list_threads = handle_print(list_threads);
            print = 0;
        }
        // Se ci sono stati fd modificati
        if (res > 0)
            // Cerco quale sia stato modificato
            for (int j = 0; j < max + 1; j++)
            {
                fflush(stdout);
                if (FD_ISSET(j, &readset))
                {
                    // Faccio la read del messaggio
                    message_t mes;
                    int read_res = read(j, &mes, sizeof(message_t));
                    ASSERTOK(read_res, "Read");
                    // Se il secret è valido
                    if (mes.secret != 0)
                    {
                        // Stampo il messaggio
                        fprintf(stdout, "SUPERVISOR ESTIMATE %" PRIu32 " FOR %" PRIx64 " FROM %d\n", mes.secret, mes.id, mes.server);
                        fflush(stdout);
                        // Avvio un thread per inserire il messaggio nella 
                        // hash table
                        pthread_t t;
                        int result = pthread_create(&t, NULL, new_data_hash_table, &mes);
                        ASSERT(result != 0, "PTHREAD");
                        // Aggiungo in testa il nuovo thread
                        list_threads = add_thread(t, list_threads);
                    }
                }
            }
    }
    // Faccio una bella pulizia di memoria
    // Valgrind approved :-)
    close_thread(list_threads);
    kill_server(k,pids);
    print_hash_table(stdout, list, HASH_DIM);
    fprintf(stdout, "SUPERVISOR EXITING\n");
    fflush(stdout);
    free(pids);
    free_arrays(pipes,file_names,i_to_s,k);
    // Pulisco la memoria dell'hash table
    for (int i = 0; i < HASH_DIM; i++) 
        if(list[i] != NULL)
            free_list(list[i]);
    // Restituisco successo
    return 0;
}
