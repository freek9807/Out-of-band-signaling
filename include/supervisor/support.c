#include"support.h"
// Variabili globali di supporto
extern volatile sig_atomic_t stop;
extern volatile sig_atomic_t print;
extern pthread_mutex_t* lock;
extern msg_list **list;
// La dimensione di un intero in stringa
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
// Stampo la lista in file
void print_list(FILE *file, msg_list *ls)
{
    // Se la lista è vuota
    if (ls == NULL)
        return;
    // Stampo ricorsivamente la lista
    print_list(file, ls->next);
    fprintf(file, "SUPERVISOR ESTIMATE %" PRIu16 " FOR %" PRIx64 " BASED ON %d\n", ls->msg.secret, ls->msg.id, ls->conn);
    fflush(file);
}
// Aggiungo un nuovo Thread alla lista
threads *add_thread(pthread_t t, threads *next)
{
    // Alloco la memoria 
    threads *node = malloc(sizeof(threads));
    ASSERT(node == NULL, "MALLOC");
    node->t = t;
    node->next = next;
    // Restituisco il valore
    return node;
}
// Libera la memoria di tuti i thread
void close_thread(threads *t)
{
    // Se la lista è vuota
    if (t == NULL)
        return;
    // Scorro la lista
    close_thread(t->next);
    // Aspetto i thread terminino
    pthread_join(t->t, NULL);
    // Libero la memoria
    free(t);
}
// Libera la memoria dei thread passati
void free_arrays(int **pipes, char **file_names, char **i_to_s, int k)
{
    // Libero la memoria degli array
    for (int i = 0; i < k; i++)
    {
        free(pipes[i]);
        free(file_names[i]);
        free(i_to_s[i]);
    }
    // Libero il primo elemento
    free(pipes);
    free(file_names);
    free(i_to_s);
}
// Uccide tuti i server
void kill_server(int k, pid_t *pids)
{
    for (int i = 0; i < k; i++)
        kill(pids[i], SIGTERM);
}
// Imposta di default i valore degli array passati
void set_array(int **pipes, char **file_names, char **i_to_s, int k)
{
    for (int i = 0; i < k; i++)
    {
        pipes[i] = malloc(sizeof(int) * 2);
        ASSERT(pipes[i] == NULL, "MALLOC");
        file_names[i] = malloc(sizeof(START_NAME) + sizeof(EXT_LOG) + dim_int_in_char(i + 1));
        ASSERT(file_names[i] == NULL, "MALLOC");
        i_to_s[i] = malloc(dim_int_in_char(i + 1) + 1);
        ASSERT(i_to_s[i] == NULL, "MALLOC");
        sprintf(i_to_s[i], "%d", i + 1);
        sprintf(file_names[i], START_NAME "%d" EXT_LOG, i + 1);
    }
}
// Genera i server
pid_t generate_server(int i, int **pipes, char **file_names, char **i_to_s)
{
    // Setto la pipe
    ASSERT(pipe(pipes[i]) == -1, "PIPE");
    // Faccio un fork
    int p = fork();
    ASSERTOK(p, "FORK");
    // Se sono nel figlio
    if (p == 0)
    {
        // Chiudo i fd precedenti al mio
        for (int j = 0; j < i; j++)
        {
            close(pipes[j][0]);
            close(pipes[j][1]);
        }
        // Chiudo la lettura
        close(pipes[i][0]);
        // Apro il file di output
        int fp = open(file_names[i], O_WRONLY | O_TRUNC | O_CREAT, 0644);
        ASSERT(fp < 0, "File");
        ASSERT(dup2(fp, 1) < 0, "DUP");
        // Faccio la dup2 della pipe
        ASSERT(dup2(pipes[i][1], 2) < 0, "DUP");
        close(fp);
        // Lancio i server
        char *args[3] = {"./server.o", i_to_s[i], NULL};
        int res = execvp(args[0], args);
        ASSERT(res < 0, "Exec");
        exit(0);
    }
    // Restituisco il pid del figlio
    return p;
}
// Imposta in set i fd e restituisce il massimo
// -1 se non c'è nulla da settare
int set_pselect_desc(int k, fd_set *set, int **pipes)
{
    int max = -1;
    FD_ZERO(set);
    // Imposto le pipes
    for (int i = 0; i < k; i++)
    {
        FD_SET(pipes[i][0], set);
        max = max < pipes[i][0] ? pipes[i][0] : max;
    }
    // Restituisco il valore
    return max;
}
// Imposta il signal handler
void set_signal_handler(struct sigaction *s, sigset_t *sigset, sigset_t *oldset)
{
    // Costtruisco la sigaction
    s->sa_handler = sigint;
    sigemptyset(&s->sa_mask);
    s->sa_flags = 0;
    // Setto la sigaction
    int res = sigaction(SIGINT, s, NULL);
    ASSERTOK(res, "SIGACTION");
    // Setto la siset
    sigemptyset(sigset);
    sigaddset(sigset, SIGINT);
    // Setto la sigprocmask
    res = sigprocmask(SIG_BLOCK, sigset, oldset);
    ASSERTOK(res, "SIGPROCMASK");
}
