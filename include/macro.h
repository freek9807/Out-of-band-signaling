#ifndef MACROS_H_
#define MACROS_H_
// Dimensione della tabella Hash in
// Supervisor
#define HASH_DIM 7
// Il nome della mia socket
#define SOCKET "OOB-server-"
// L'estensione della mia socket
#define EXT ".sock"
// Estensione dei file di log
#define EXT_LOG ".log"
// File contenenti gli output dei server
#define START_NAME "SERVER_"
// Se la condizione e viene soddisfatta l'esecuzione
// continua
#define ASSERT(e, m) \
  if (e)             \
  {                  \
    printf(m);       \
    exit(-1);        \
  }
// Se la condizione è diversa da -1 continua
#define ASSERTOK(e, m) \
  if (e == -1)         \
  {                    \
    printf(m);         \
    exit(-1);          \
  }
// Sugar code per allocare memoria
#define ALLOC(d, t) (t) malloc(d)
// Dimensione massima del path
#define UNIX_PATH_MAX 108
// Posix set of features
#define _POSIX_C_SOURCE 200809L
#endif