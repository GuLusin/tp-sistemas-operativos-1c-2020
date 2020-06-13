#ifndef GAMECARD_H
#define GAMECARD_H

#include <utils/conexiones.h>
#include <utils/mensajes.h>
#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/txt.h>
#include <pthread.h>
#include <semaphore.h>
#include <commons/collections/list.h>

pthread_mutex_t mutex_recibir;

t_config* config;
t_log* logger;
int tiempo_reintento_conexion, tiempo_reintento_operacion,
	tiempo_retardo_operacion, socket_broker, socket_cola_new,
	socket_cola_catch, socket_cola_get;
char *ip_broker, *puerto_broker, *punto_montaje;

typedef struct{
	int directory;
	int size;
	int* blocks;
	int open;
}t_metadata;

#endif /* GAMECARD_H */
