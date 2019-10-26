#include"../include/server/support.h"

int main(int argc, char *argv[])
{
    // Setto le variabili di supporto 
    client *cl = NULL;
    char *s1;
    int count = 0;
    int i = atoi(argv[1]);
    ASSERT( i == 0 , "Argomenti non validi \n");
    int fd_skt, fd_c, fd_num = 0;
    int result;
    uint64_t id_nbo;
    fd_set readfds;
    // Setto il segnale di terminazione
    sigset_t oldset;
    set_signal(&oldset);
    // Alloco la memoria con il nome del server
    s1 = ALLOC(sizeof(SOCKET) + sizeof(EXT) + sizeof(i) + 1, char *);
    ASSERT(s1 == NULL, "MALLOC");
    sprintf(s1, SOCKET "%d" EXT, i);
    // Elimino il precedente file del server
    unlink(s1);
    // Imposto tutti i valori della struct
    struct sockaddr_un sa;
    set_socket(&fd_skt, &sa, s1);
    // Attivo il server
    printf("SERVER %d ACTIVE\n", i);
    fflush(stdout);
    while (1)
    {
        // Setto i fd della select
        set_descriptor_select(&readfds, fd_skt, &fd_num, cl, count);
        // Mi metto in ascolto sui fd con la pselect
        result = pselect(fd_num + 1, &readfds, NULL, NULL, NULL, &oldset);
        // Controllo il valore restituito dalla pselect
        check_pselect(result, cl, count, s1);
        // Se il messaggio è sulla socket vuol dire nuova connessione
        if (FD_ISSET(fd_skt, &readfds))
        { 
            // Accetto la nuova connessione 
            ASSERTOK((fd_c = accept(fd_skt, NULL, 0)), "Errore accept");
            // Ricevo il mio id
            int n = recv(fd_c, &id_nbo, sizeof(uint64_t), 0);
            ASSERTOK(n, "Errore nella ricezione del messaggio");
            ASSERT(n == 0, "Errore , Messaggio vuoto");
            // Ottengo il messaggio originale
            uint64_t id = get_id_from_nbo(id_nbo);
            // Aggiungo un nuovo client
            cl = realloc(cl, sizeof(client) * ++count);
            ASSERT(cl == NULL, "REALLOC");
            // Ottengo il tempo
            uint64_t time = get_clocktime();
            // Aggiungo un nuoov client
            new_client(id, cl, count - 1, fd_c,time);
            //Stampo
            printf("SERVER %d CONNECT FROM CLIENT\n", i);
            printf("SERVER %d INCOMING FROM %" PRIx64 " @ %lu\n", i, id, time);
            fflush(stdout);
        }
        // Cerco se il descrittore è fra quelli settati
        for (int j = 0; j < count; j++)
            if (FD_ISSET(cl[j].descriptor, &readfds))
            {
                // Ricevo il messaggio
                int n = recv(cl[j].descriptor, &id_nbo, sizeof(uint64_t), 0);
                ASSERTOK(n, "Errore nella ricezione del messaggio");
                // Gestisco il messaggio
                handle_client_message(n, i, id_nbo, get_clocktime(), cl, j, &readfds);
            }
    }
}