#include "hash_table.h"
// Hash table dei messaggi
extern msg_list **list;
// Calcolo il valore di hash
// (Si poteva fare di meglio , lo so )
int calculate_hash_vl(uint64_t val,int dim)
{
    return val % dim;
}
// Aggiungo un valore alla tabella hash
msg_list *add_hash_table(message_t msg, msg_list *lm, int conn)
{
    // Se il secret è 0 
    // Quindi inutile lo ignoro
    if(msg.secret == 0)
        return lm;
    // Inserisco un nuovo nodo alla lista    
    msg_list *ls = malloc(sizeof(msg_list));
    ASSERT(ls == NULL , "MALLOC");
    ls->conn = conn;
    ls->msg = msg;
    ls->next = lm;
    return ls;
}

msg_list *search_hash_table(message_t msg, msg_list *lm)
{
    // Se la lista è nulla aggiungo
    if (lm == NULL)
        return add_hash_table(msg, lm, 0);
    // Se ho già una stima di quel messaggio
    if (lm->msg.id == msg.id)
    {
        // Se la stima non è zero ed è minore
        if (lm->msg.secret > msg.secret && msg.secret != 0)
        {   
            // Aggiungo
            lm = add_hash_table(msg, lm->next, lm->conn + 1);
            return lm;
        }
        else
        {
            // Aumento il numero di connessioni
            lm->conn += 1;
            return lm;
        }
    }
    // Continuo la ricerca
    lm->next = search_hash_table(msg, lm->next);
    return lm;
}

void free_list(msg_list *list)
{
    // Se la lista è vuota
    if (list == NULL)
        return;
    // Scorro tutta la lista
    free_list(list->next);
    // Libero il valore
    free(list);
}
