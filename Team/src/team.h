#ifndef TEAM_H
#define TEAM_H

#include "../../utils/conexiones.c"
#include "../../utils/mensajes.c"
#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <pthread.h>

t_config* config;
t_log* logger;

char *ip_broker,*puerto_broker;

int wait_time;

pthread_mutex_t send_mutex;

#endif /* TEAM_H */


