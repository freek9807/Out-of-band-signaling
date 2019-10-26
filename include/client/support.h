#ifndef _support_client_h
#define _support_client_h
// Le librerie che uso
#include "../macro.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<inttypes.h>
#include<time.h>
#include <sys/un.h> 
#include <netinet/in.h>
#include <unistd.h>
// La struttura che descrive una connessione
// a un server
typedef struct {
    uint32_t value;
    int active;
    int descriptor;
} server;
// Controlla se un valore si trova nell'array
int is_in_array(server*,int,int,uint32_t);
// Guarda la dimensione di un intero in stringa
int dim_int_in_char(uint32_t);
// Restituisce la dimensione del nome del server
int size_of_server_name(uint32_t);
// Genera il secret
uint16_t generate_secret();
// Genera l'id
uint64_t generate_id();
// Popola il server degli arrays
void populate_server_array(server*,uint32_t,uint32_t);
// Genera il valore in nbo
uint64_t generate_nbo(uint64_t);
// Chiude tutti i descrittori 
void close_des(server*,uint32_t);
// Invia il messaggio al server
void sender(uint64_t,uint32_t,uint32_t,uint32_t,server*,struct timespec);

#endif