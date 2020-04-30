#ifndef BROKER_H
#define BROKER_H

#include "back_end_broker.c"
#include <pthread.h>



t_log* logger;
t_config* config;
t_list* sockets_cola_new;
t_list* sockets_cola_get;
t_list* sockets_cola_catch;
t_list* sockets_cola_localized;
t_list* sockets_cola_caught;
t_list* sockets_cola_appeared;


#endif /* BROKER_H */





