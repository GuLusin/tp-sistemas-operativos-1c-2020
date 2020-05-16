#ifndef TEAM_H
#define TEAM_H

#include <utils/conexiones.h>
#include <utils/mensajes.h>
#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <pthread.h>
#include <commons/collections/list.h>


typedef struct {
   int id;
   int posicion_x;
   int posicion_y;
   t_list *pokemones;
   t_list *objetivos;
}t_entrenador;

int mapa[25][25];

t_config* config;
t_log* logger;

t_list* new_entrenadores;
t_list* ready_entrenadores;
t_list* exec_entrenadores;
t_list* block_entrenadores;
t_list* exit_entrenadores;

char *ip_broker,*puerto_broker;

int wait_time;

pthread_mutex_t send_mutex;

#endif /* TEAM_H */


