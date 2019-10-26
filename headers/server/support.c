#include"support.h"
// Funzione di signal handling
// Mi serve solo a segnalare il SIGTERM 
// Alla pselect
void sigterm(int signo)
{
    return;
}
// Funzione che setta il signal handler
void set_signal(sigset_t *oldset)
{
    // Costruisco la sigaction
    struct sigaction s;
    s.sa_handler = sigterm;
    sigemptyset(&s.sa_mask);
    s.sa_flags = 0;
    // Imposto la sgaction al SIGTERM
    int res = sigaction(SIGTERM, &s, NULL);
    ASSERTOK(res,"SIGACTION");
    // Costruisco il sigset
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGTERM);
    // Costruisco il lsigprocmask
    res = sigprocmask(SIG_BLOCK, &sigset, oldset);
    ASSERTOK(res, "SIGPROCMASK");
}
// Funzione che costruisce la socket
void set_socket(int *fd_skt, struct sockaddr_un *sa, char *s1)
{
    // Istanzio una nuova socket
    *fd_skt = socket(AF_UNIX, SOCK_STREAM, 0);
    ASSERTOK((*fd_skt), "Creazione socket");
    // Imposto le informazioni della socket
    // Il nome
    strncpy(
        sa->sun_path,
        s1,
        UNIX_PATH_MAX);
    // Il dominio
    sa->sun_family = AF_UNIX;
    // Faccio la bind
    int res = bind(
        *fd_skt,
        (struct sockaddr *)sa,
        sizeof(*sa));
    ASSERTOK(res,"BIND");
    // Ascolto per una connessione 
    // sulla socket
    res = listen(*fd_skt, SOMAXCONN);
    ASSERTOK(res,"LISTEN");
}
// Funzione che setta i fd della socket
void set_descriptor_select(fd_set *readfds, int fd_skt, int *fd_num, client *cl, int count)
{
    // Azzero il set di fd
    FD_ZERO(readfds);
    // Imposto la main socket
    FD_SET(fd_skt, readfds);
    // Il massimo valore su cui ascoltare
    *fd_num = fd_skt;
    // Aggiungo gli altri fd
    for (int k = 0; k < count; k++){
        // Li aggiungo se attivi
        if (cl[k].active == 1)
        {
            FD_SET(cl[k].descriptor, readfds);
            // Guardo per un nuovo valore max
            if (cl[k].descriptor > *fd_num)
                *fd_num = cl[k].descriptor;
        }
    }   
}
// Controlla la validità della socket
void check_pselect(int result, client *cl, int count, char *s1)
{
    // Se il valore della select è
    //maggiore di 0 non faccio nulla
    if (result >= 0)
        ;
    // Allora gestisco il segnale e termino
    else if (errno == EINTR)
    {
        for (int i = 0; i < count; i++)
            close(cl[i].descriptor);
        free(cl);
        free(s1);
        fflush(stdout);
        exit(-1);
    }
    // Gestisco l'errore
    else
        ASSERT(result < 0, "Errore nella select !");
}
// Ottengo il tempo in microsecondi
uint64_t get_clocktime()
{
    // Ottengo il tempo
    struct timespec tms;
    if (clock_gettime(CLOCK_REALTIME, &tms))
    {
        exit(-1);
    }
    // Ottengo i secondi
    int64_t micros = tms.tv_sec * 1000000;
    // I nanosecondi
    micros += tms.tv_nsec / 1000;
    // Arrotondo
    if (tms.tv_nsec % 1000 >= 500)
    {
        ++micros;
    }
    // Restituisco il tempo
    return micros;
}
// Ottengo l'id in nbo
uint64_t get_id_from_nbo(uint64_t id_nbo)
{
    return 
        (uint64_t)htonl((id_nbo << 32) >> 32) << 32 | htonl(id_nbo >> 32);
}
// Costruisco un nuovo client da cui ascoltare sul fd
void new_client(uint64_t id, client *cl, int count, int fd_c, uint64_t time)
{
    cl[count].descriptor = fd_c;
    cl[count].active = 1;
    cl[count].id = id;
    cl[count].prev = time;
    cl[count].estimate = 0;
    cl[count].num_conn = 1;
}
// Ottengo la dimensione di un intero in stringa
int dim_int_in_char(uint32_t value)
{
    int count = 0;
    while (value != 0)
    {
        value /= 10;
        ++count;
    }
    return count * sizeof(char);
}
// Gestisco un nuovo messaggio da un client
void handle_client_message(int n, int i, uint64_t id_nbo, uint64_t time, client *cl, int j, fd_set *readfds)
{
    // Se il numero di bytes letti è zero allora 
    // è stato chiuso il fd
    if (n == 0)
    {
        // Costruisco il messaggio da stampare in stderr
        message_t message_s = {cl[j].id , cl[j].estimate, i};
        // Lo scrivo
        int res = write(2,&message_s,sizeof(message_t));
        ASSERTOK(res,"WRITE");
        // Stampo il messaggio di chiusura
        int message_dim = snprintf(NULL, 0, "ID : %" PRIx64 " ESTIMATE : %" PRIu32 "\n", cl[j].id, cl[j].estimate);
        char* message = malloc(message_dim + 1);
        ASSERT(message == NULL, "MALLOC");
        sprintf(message, "ID : %" PRIx64 " ESTIMATE : %" PRIu32 "\n", cl[j].id, cl[j].estimate);
        printf("SERVER %d CLOSING %" PRIx64 " ESTIMATE %d\n", i, cl[j].id, cl[j].estimate);
        fflush(stdout);
        // Elimino il fd
        FD_ZERO(readfds);
        // Lo disattivo
        cl[j].active = 0;
        // Libero la memoria
        free(message);
    }
    else
    {
        // Ottengo l'id in nbo
        uint64_t id = get_id_from_nbo(id_nbo);
        // Ottengo una nuova stima
        int discard = (time - cl[j].prev) / 1000;
        cl[j].prev = time;
        // Valuto se è attendibile come stima
        cl[j].estimate = (discard < cl[j].estimate || !cl[j].estimate) ? discard : cl[j].estimate;
        ++cl[j].num_conn;
        // Stampo il valore
        printf("SERVER %d INCOMING FROM %" PRIx64 " @ %lu \n", i, id, cl[j].prev);
        fflush(stdout);
    }
}
