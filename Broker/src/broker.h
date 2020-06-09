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

#define CANTIDAD_COLAS 6

pthread_mutex_t mutex_recibir;
pthread_mutex_t mutex_enviar;
pthread_mutex_t* mutex_cola_suscriptores;

t_log* logger;
t_config* config;
int socket_broker;

typedef struct{
	int id_team;
	int socket_cliente;
}t_suscriptor;

t_list** suscriptores;

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
	void* inicio;
	int size;
	int msg_id;
	int cola_code;
	int id_correlativo;
	t_list* suscriptores_confirmados;
}t_partition;

typedef struct{
	void* inicio;
	int size;
}t_free_partition;

int TAMANO_MEMORIA;
algoritmo_particion_libre APL;
algoritmo_reemplazo AR;


pthread_mutex_t mutex_particiones_libres;

t_list* particiones_libres; 	//LISTA CON LAS PARTICIONES LIBRES DISPONIBLES.
adm_cola* administracion_colas; // ARREGLO DE LAS 6 COLAS QUE EL BROKER ADMINISTRA EN LA CACHE.
void* mem_alloc; 				//ESTE PUNTERO ES GLOBAL Y UNICO, NO SE TOCA NI MODIFICA. ES EL PRINCIPIO DE LA CACHE.



void asignar_id_correlativo_a_particion(t_partition* particion,t_mensaje* mensaje);
void cachear_mensaje(t_mensaje* mensaje_aux);
void unificar_particiones_libres();
int puntero_cmp(void* un_puntero, void* otro_puntero);
bool particiones_libres_contiguas(t_free_partition* particion1,t_free_partition* particion2);
bool es_suscriptor_confirmado(t_list* lista,int socket_cliente);
t_mensaje* get_mensaje_cacheado(int cola_code, int index);
t_mensaje* leer_cache(void* stream);
t_mensaje* leer_particion_cache(t_partition* particion);
t_suscriptor* crear_suscriptor(int id, int socket_cliente);
char* cola_string(int cola);
t_partition* encontrar_particion(int id,int cola,int* posicion);
t_mensaje* descachear_mensaje(int msg_id,int cola);
void sacar_particion(int cola, int index);





#endif /* BROKER_H */





