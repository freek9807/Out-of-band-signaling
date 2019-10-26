#ifndef _support_server_h
#define _support_server_h

#include "../macro.h"
#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include <sys/un.h>
#include <inttypes.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
// Struttura di un cliente 
typedef struct client
{
    int descriptor;
    int active;
    uint64_t prev;
    uint64_t id;
    int estimate;
    int num_conn;
} client;
// Struttura di un messaggio al supervisor
typedef struct message{
    uint64_t id;
    uint32_t secret;
    int server;
} message_t;
// Funzione di signal handling
void sigterm(int);
// Funzione che setta il signal handler
void set_signal(sigset_t*);
// Funzione che costruisce la socket
void set_socket(int*,struct sockaddr_un*,char*);
// Funzione che setta i fd della socket
void set_descriptor_select(fd_set*,int,int*,client*,int);
// Controlla la validità della socket
void check_pselect(int,client*,int,char*);
// Ottengo il tempo in microsecondi
uint64_t get_clocktime();
// Ottengo la dimensione di un intero in stringa
int dim_int_in_char(uint32_t value);
// Ottengo l'id in nbo
uint64_t get_id_from_nbo(uint64_t);
// Costruisco un nuovo client da cui ascoltare sul fd
void new_client(uint64_t,client*,int,int,uint64_t);
// Gestisco un nuovo messaggio da un client 
void handle_client_message(int,int,uint64_t,uint64_t,client*,int,fd_set*);
#endif