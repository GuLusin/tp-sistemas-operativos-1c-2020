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
#include <commons/process.h>
#include <commons/temporal.h>
#include <commons/collections/list.h>
#include <commons/bitarray.h>
#include <math.h>
#include <pthread.h>
#include <signal.h>
#include <semaphore.h>

#define CANTIDAD_COLAS 6

typedef struct{
	int id_team;
	int socket_cliente;
}t_suscriptor;

int socket_broker;
int id_mensajes_globales;

t_log* logger;
t_config* config;
t_list** suscriptores;

//=========================COORDINACION BROKER================================

pthread_mutex_t mutex_id_mensajes_globales;
pthread_mutex_t mutex_logger;
pthread_mutex_t mutex_lista_algoritmo_reemplazo;
pthread_mutex_t mutex_particiones_libres;
pthread_mutex_t mutex_recibir;
pthread_mutex_t mutex_enviar;
pthread_mutex_t mutex_particiones_ocupadas;
pthread_mutex_t* mutex_cola_suscriptores;
pthread_mutex_t* mutex_administracion_colas;

sem_t sem_recibir;
sem_t sem_memoria;
sem_t sem_suscriptores;
sem_t esperar;

//========================PARTICIONES DINAMICAS===============================

typedef enum{
	BS,
	PARTICIONES,
}algoritmo_memoria;

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
	int fragmentacion_interna;
	t_list* suscriptores_confirmados;
	char tipo;
}t_partition;

int tamanio_memoria;
int tamanio_minimo_particion;
int freq_compactacion;
int particiones_ocupadas;

algoritmo_particion_libre APL;
algoritmo_reemplazo AR;
algoritmo_memoria AM;

t_list* lista_algoritmo_reemplazo;
t_list* particiones_libres; 	//LISTA CON LAS PARTICIONES LIBRES DISPONIBLES.
adm_cola* administracion_colas; // ARREGLO DE LAS 6 COLAS QUE EL BROKER ADMINISTRA EN LA CACHE.
void* mem_alloc; 				//ESTE PUNTERO ES GLOBAL Y UNICO, NO SE TOCA NI MODIFICA. ES EL PRINCIPIO DE LA CACHE.


//==============================BUDDY SYSTEM ==============================

typedef struct{
	int largo;
	int peso_maximo;
	int peso_minimo;
	t_bitarray** bitmap_array;
}t_bitmap;

t_bitmap* bitmap;

//================================FUNCIONES===================================

void printear_estado_memoria();
void asignar_id_correlativo_a_particion(t_partition* particion,t_mensaje* mensaje);
void unificar_particiones_libres();
void descachear_mensaje(int msg_id,int cola);
void sacar_particion(int cola, int index);
void confirmar_suscriptor(t_suscriptor* suscriptor,t_mensaje* mensaje);
void remover_suscriptor(t_suscriptor* suscriptor,t_mensaje* mensaje);
void validar_suscriptor(t_suscriptor* suscriptor, t_mensaje* mensaje_aux);
void compactar_memoria();
void notificar_mensaje(t_mensaje* mensaje);
void particion_a_bitmap(t_partition* particion, int* indice, int* pos);
void partir_particion(t_bitmap* bitmap, int indice, int pos);
void bitmap_clean(t_bitmap* bitmap);
void bitmap_actualizar_particion(t_partition* particion);
void dividir_particion(t_partition* particion);
void agregar_pokemon_a_especie(t_pokemon_especie* pokemon_especie, t_pokemon* pokemon);
void* pmalloc_BS(int size);
char* cola_string(int cola);
int encontrar_particion(int msg_id,int cola);
int puntero_cmp(void* un_puntero, void* otro_puntero);
int asignar_buddy_libre(t_bitmap* bitmap,int size);
int bitmap_a_puntero(int indice,int pos);
bool particiones_libres_contiguas(t_partition* particion1,t_partition* particion2);
bool es_suscriptor_confirmado(t_partition* particion,int id_team);
bool es_suscriptor_cola(int cola, t_suscriptor* suscriptor);
bool bit_test(t_bitarray* bitarray, int index);
bool condicion_bs(t_partition* una_particion, t_partition* otra_particion);
t_mensaje* get_mensaje_cacheado(int cola_code, int index);
t_mensaje* leer_cache(void* stream);
t_mensaje* leer_particion_cache(t_partition* particion);
t_suscriptor* crear_suscriptor(int id, int socket_cliente);
t_partition* cachear_mensaje(t_mensaje* mensaje_aux);
t_partition* encontrar_particion_libre_por_ptr(int puntero_relativo);

#endif /* BROKER_H */



