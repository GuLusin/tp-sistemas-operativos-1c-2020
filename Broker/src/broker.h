#ifndef BROKER_H
#define BROKER_H

#include <utils/conexiones.h>
#include <utils/mensajes.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <pthread.h>


pthread_mutex_t mutex_cola_new;
pthread_mutex_t mutex_cola_get;
pthread_mutex_t mutex_cola_catch;
pthread_mutex_t mutex_cola_localized;
pthread_mutex_t mutex_cola_caught;
pthread_mutex_t mutex_cola_appeared;


t_log* logger;
t_config* config;

t_list* sockets_cola_new;
t_list* sockets_cola_get;
t_list* sockets_cola_catch;
t_list* sockets_cola_localized;
t_list* sockets_cola_caught;
t_list* sockets_cola_appeared;





#endif /* BROKER_H */





