#ifndef _support_server_h
#define _support_server_h
#include "../../headers/macro.h"
#include "../../headers/hash_table/hash_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <inttypes.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
// Struttura di  una lista di 
//threads 
typedef struct threads_list
{
    pthread_t t;
    struct threads_list *next;
} threads;
// La dimensione di un intero in stringa
int dim_int_in_char(uint32_t);
// Stampa una lista di messaggi
void print_list(FILE *, msg_list *);
// Aggiunge un thread alla lista
threads *add_thread(pthread_t,threads*);
// Aspetta la fine di tutti i thread
void close_thread(threads*);
// Libera la memoria di tutti gli array
void free_arrays(int **, char **, char **, int);
// Uccide tutti i server
void kill_server(int , pid_t *);
// Setta gli array con i valori corretti
void set_array(int **, char **, char **, int);
// Genera i server
pid_t generate_server(int, int **, char **, char **);
// Setta i fd della select
int set_pselect_desc(int, fd_set *, int **);
// Setta il signal handler
void set_signal_handler(struct sigaction *, sigset_t *, sigset_t *);
// Stampo l'ht 
void print_hash_table(FILE *file, msg_list **ls, int dim) ;
// Stampo la lista della di messaggi
void *print_ls(void *args);
// Funzione di sh
void sigint(int signo);
// Aggiungo un elemnto alla tabella hash
void *new_data_hash_table(void *val);
// Setto i valori della ht
void set_hash_table();
// Aggiungo un thread per la stampa 
threads *handle_print(threads *list_threads);

#endif