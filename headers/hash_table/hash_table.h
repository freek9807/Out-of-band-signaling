#ifndef HASH_TABLE_MSG_H_
#define HASH_TABLE_MSG_H_
#include "../macro.h"
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <pthread.h>
// Struttura contenente il corpo 
// di un messaggio
typedef struct message
{
    uint64_t id;
    uint32_t secret;
    int server;
} message_t;
// Lista di messaggi
typedef struct msg_list
{
    message_t msg;
    int conn;
    struct msg_list *next;
} msg_list;
// Aggiunge un nuovo elemento alla hash table
msg_list *add_hash_table(message_t, msg_list*, int);
// Calcola il valore di hash
int calculate_hash_vl(uint64_t, int);
// Libera la memoria di una lista della ht
void free_list(msg_list *);
// Cerca e aggiunge un valore
msg_list *search_hash_table(message_t, msg_list*);
#endif