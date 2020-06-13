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
#include <semaphore.h>
#include <commons/collections/list.h>

typedef struct {
   int id;
   int posicion_x;
   int posicion_y;
   int bloq_exec;
   int exit;
   t_pokemon *objetivo_temporal;
   t_list *pokemones;
   t_list *objetivos;
}t_entrenador;

t_config* config;
t_log* logger;

char *ip_broker,*puerto_broker;
int wait_time;
bool broker_conectado;

//---------------------------------- PLANIFICACION -----------------------------------------------

typedef enum {
	FIFO,
	RR,
	SJF_CD,
	SJF_SD,
} algoritmo_code;

int retardo;
uint32_t* respuesta_caught;

//-----------------LISTAS

t_list* list_pok_new;
t_list* list_pok_ready;
t_list* entrenadores;
t_list* lista_corto_plazo;

//-----------------DICCIONARIOS

t_dictionary* dic_pok_obj;
t_dictionary* dic_pok_ready_o_exec;
t_dictionary* ids_a_esperar_localized; //localized
t_dictionary* ids_a_esperar_catch;

//-----------------SEMAFOROS

sem_t *ejecutar_entrenador; //vector
sem_t *deadlock_entrenadores; //vector
sem_t *espera_caught; //vector
sem_t sem_deadlock;
sem_t activar_algoritmo;
sem_t hayentrenadorlibre;
sem_t revisar_pokemones_new;
sem_t hay_pokemones;
sem_t hay_entrenador_disponible;
sem_t hay_entrenador_corto_plazo;
sem_t entrenador_bloqueado;

//-----------------MUTEX

pthread_mutex_t send_mutex;
pthread_mutex_t mutex_recibir;
pthread_mutex_t list_pok_new_mutex;
pthread_mutex_t list_pok_ready_mutex;
pthread_mutex_t mutexPRUEBA;
pthread_mutex_t mutex_pokemones_recibidos;
pthread_mutex_t mutex_lista_entrenadores;
pthread_mutex_t mutex_lista_corto_plazo;
pthread_mutex_t mutex_confirmacion;
pthread_mutex_t mutex_respuesta;
pthread_mutex_t mutex_logger;

//-----------------HILOS

pthread_t *hilo_entrenador;

#endif /* TEAM_H */
