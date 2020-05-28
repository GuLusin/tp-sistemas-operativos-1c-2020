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

#define CODIGO_OPERACION_DEFAULT -1

typedef struct {
   int id;
   int posicion_x;
   int posicion_y;
   t_pokemon *objetivo_temporal;
   t_list *pokemones;
   t_list *objetivos;
}t_entrenador;

t_config* config;
t_log* logger;

char *ip_broker,*puerto_broker;

int wait_time;

pthread_mutex_t send_mutex;

//---------------------------------- PLANIFICACION -----------------------------------------------

int retardo;
int largo_lista_conocida;

typedef enum {
	FIFO,
	RR,
	SJF_CD,
	SJF_SD,
} algoritmo_code;

//sem_t *semaforos_entrenadores; //comienza con el id 0, para q corresponda con el index de cada lista

sem_t *ejecutar_entrenador; //vector
sem_t activar_algoritmo;

t_list* entrenadores; //new - bloqueado - deadlock?
t_entrenador entrenador_deadlock_1; //mutex para recorrer la lista, hay dos q pasen a deadlock
t_entrenador entrenador_deadlock_2; //sino cada vez q se atrapa uno se habilita otra iteracion

t_list* lista_corto_plazo; //lista de entrenadores para moverse

//t_list *sobra;
//t_list *faltan;

t_list* pokemons_recibidos;
sem_t hay_pokemones;
sem_t hay_entrenador_corto_plazo;
sem_t cumplio_objetivo_global;

//Bool vectorConfirmacionCatch;

pthread_mutex_t mutex_pokemones_recibidos;


int cantidad_pokemons_globales;
int cantidad_objetivos_globales;
#endif /* TEAM_H */
