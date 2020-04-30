#ifndef BROKER_H
#define BROKER_H

#include "../../utils/conexiones.c"
#include "../../utils/mensajes.c"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <pthread.h>

pthread_mutex_t recv_mutex;

t_log* logger;
t_config* config;

t_list* sockets_cola_new;
t_list* sockets_cola_get;
t_list* sockets_cola_catch;
t_list* sockets_cola_localized;
t_list* sockets_cola_caught;
t_list* sockets_cola_appeared;





#endif /* BROKER_H */





