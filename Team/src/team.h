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
#include <commons/collections/list.h>}

typedef enum {
	NEW,
	READY,
	EXEC,
	BLOCK,
	EXIT,
}estado_entrenador;

typedef struct {
   int posicion_x;
   int posicion_y;
   t_list *pokemones;
   t_list *objetivos;
   estado_entrenador estado;
}t_entrenador;

t_config* config;
t_log* logger;

t_list* entrenadores;

char *ip_broker,*puerto_broker;

int wait_time;

pthread_mutex_t send_mutex;

#endif /* TEAM_H */


