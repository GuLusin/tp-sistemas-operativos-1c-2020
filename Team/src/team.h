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
   char* nombre;
   int pos_x;
   int pos_y;
}t_pokemon;

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

typedef enum {
	FIFO,
	RR,
	SJF_CD,
	SJF_SD,
} algoritmo_code;

//sem_t *semaforos_entrenadores; //comienza con el id 0, para q corresponda con el index de cada lista

pthread_mutex_t *ejecutar_entrenador; //vector

t_list* entrenadores;
t_list* lista_corto_plazo; //lista de entrenadores para moverse

//t_list *sobra;
//t_list *faltan;

t_list* pokemons_recibidos;
//sem_t hay_pokemones;

int cantidad_pokemons_globales;
int cantidad_objetivos_globales;
#endif /* TEAM_H */
