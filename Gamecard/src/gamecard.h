#ifndef GAMECARD_H
#define GAMECARD_H

#include <stdio.h>
#include <stdlib.h>

#include <utils/conexiones.h>
#include <utils/mensajes.h>

#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/txt.h>
#include <commons/collections/list.h>
#include <commons/bitarray.h>

#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h> //esto es para el mmap

#include <errno.h>

pthread_mutex_t mutex_recibir;
pthread_mutex_t mutex_bitmap;
pthread_mutex_t mutex_metadata;

t_config* config;

t_log* logger;
int tiempo_reintento_conexion, tiempo_reintento_operacion,
	tiempo_retardo_operacion, socket_broker, socket_cola_new,
	socket_cola_catch, socket_cola_get, id_gamecard;
char *ip_broker, *puerto_broker, *punto_montaje;

t_bitarray* bitmap;

typedef struct{
	int directory;
	int size;
	t_list* blocks;
	int opened;
}t_metadata;

typedef struct{
	int block_size;
	int blocks;
	char* magic_number;
}t_global_metadata;

t_global_metadata* global_metadata;

#endif /* GAMECARD_H */
