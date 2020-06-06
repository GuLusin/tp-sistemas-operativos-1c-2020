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
#include <semaphore.h>


pthread_mutex_t mutex_cola_new;
pthread_mutex_t mutex_cola_get;
pthread_mutex_t mutex_cola_catch;
pthread_mutex_t mutex_cola_localized;
pthread_mutex_t mutex_cola_caught;
pthread_mutex_t mutex_cola_appeared;


t_log* logger;
t_config* config;
int socket_broker;


t_list* sockets_cola_new;
t_list* sockets_cola_get;
t_list* sockets_cola_catch;
t_list* sockets_cola_localized;
t_list* sockets_cola_caught;
t_list* sockets_cola_appeared;


int id_mensajes_globales;


//========================PARTICIONES DINAMICAS===============================


typedef enum{
	FF,
	BF,
}algoritmo_particion_libre;

typedef enum{
	FIFO,
	LRU,
}algoritmo_reemplazo;

typedef struct{
	t_list* particiones;
}adm_cola;

typedef struct{
	int pid;
	// partition id; seria bueno que corresponda con el id de su mensaje?
	//que relacion hay entre cada adm_cola con cada t_partition
	void* inicio;
	int size;
	t_list* suscriptores_confirmados; // como identificamos a un suscriptor? por el socket? es temporal?
}t_partition;

typedef struct{
	void* inicio;
	int size;
}t_free_partition;

#define CANTIDAD_COLAS 6
int TAMANO_MEMORIA;
algoritmo_particion_libre APL;
algoritmo_reemplazo AR;


pthread_mutex_t mutex_particiones_libres;

t_list* particiones_libres; 	//LISTA CON LAS PARTICIONES LIBRES DISPONIBLES.
adm_cola* administracion_colas[5]; // ARREGLO DE LAS 6 COLAS QUE EL BROKER ADMINISTRA EN LA CACHE.
void* mem_alloc; //ESTE PUNTERO ES GLOBAL Y UNICO, NO SE TOCA NI MODIFICA. ES EL PRINCIPIO DE LA CACHE.


int puntero_cmp(void* un_puntero, void* otro_puntero);
bool particiones_libres_contiguas(t_free_partition* particion1,t_free_partition* particion2);
bool es_suscriptor_confirmado(t_list* lista,int socket_cliente);
t_mensaje* get_mensaje_cacheado(int cola_code, int index);
t_mensaje* leer_cache(void* stream);
#endif /* BROKER_H */





