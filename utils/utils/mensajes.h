/*
 * conexiones.h
 *
 *  Created on: Apr 20, 2020
 *      Author: madd
 */

#ifndef MENSAJES_H
#define MENSAJES_H
#include "conexiones.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <pthread.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include <commons/string.h>

#define ID_SUSCRIPCION 999999
#define ID_DEFAULT 888888
#define CODIGO_OPERACION_DEFAULT -1

typedef enum {
	COLA_NEW_POKEMON,
	COLA_GET_POKEMON,
	COLA_CATCH_POKEMON,
	COLA_CAUGHT_POKEMON,
	COLA_LOCALIZED_POKEMON,
	COLA_APPEARED_POKEMON,
} cola_code;

typedef enum
{
	NEW_POKEMON,
	GET_POKEMON,
	CATCH_POKEMON,
	CAUGHT_POKEMON,
	LOCALIZED_POKEMON,
	APPEARED_POKEMON,
	SUBSCRIPCION,
} op_code;

typedef struct {
	char* nombre; // SE USA STRDUP
	uint32_t pos_x;
	uint32_t pos_y;
}t_pokemon;

typedef struct{
	char* nombre_especie;
	t_dictionary* posiciones_especie;
}t_pokemon_especie;

typedef struct{
	uint32_t id_correlativo;
	t_pokemon* pokemon;
}t_appeared_pokemon;

typedef struct{
	char* nombre_pokemon;
}t_get_pokemon; //ESTE MENSAJE ESPERA COMO CONFIRMACION UN ID QUE SERA EL CORRELATIVO PARA CONFIRMACION DE LA RECEPCION DE APPEARED O LOCALIZED

typedef struct{
	t_pokemon* pokemon;
	uint32_t cantidad;
}t_new_pokemon;

typedef struct{
	t_pokemon* pokemon;
}t_catch_pokemon;

typedef struct{
	uint32_t id_correlativo;
	uint32_t caught_confirmation;
}t_caught_pokemon;

typedef struct{
	uint32_t id_correlativo;
	t_pokemon_especie* pokemon_especie;
}t_localized_pokemon;

typedef struct{
	uint32_t id;
	uint32_t codigo_operacion;
	union{
		uint32_t subscripcion;
		t_get_pokemon get_pokemon;
		t_appeared_pokemon appeared_pokemon;
		t_new_pokemon new_pokemon;
		t_catch_pokemon catch_pokemon;
		t_caught_pokemon caught_pokemon;
		t_localized_pokemon localized_pokemon;
	}contenido;
}t_mensaje;

//size se envia en la serializacion aunque no este en el struct



t_mensaje* crear_mensaje(int num, ...);
int enviar_mensaje(int socket_a_enviar, t_mensaje* mensaje);
void* serializar_subscripcion(cola_code cola);
cola_code deserializar_subscripcion(void* stream);
t_mensaje* deserializar_mensaje(int codigo_operacion, void* stream);
char* especie_pokemon_a_string(t_pokemon_especie* pokemon_especie);

void printear_pokemon(t_pokemon* pokemon);
void printear_mensaje(t_mensaje* mensaje);


t_pokemon* crear_pokemon(char* nombre,uint32_t px, uint32_t py);
void* serializar_pokemon(t_pokemon* pokemon);
t_pokemon* deserializar_pokemon(void* stream);
int tamanio_pokemon(t_pokemon* pokemon);
char* posiciones_a_string(t_dictionary* posiciones);
char* posicion_string_pokemon(t_pokemon* pokemon);



t_pokemon_especie* deserializar_pokemon_especie(void* string);

#endif /* MENSAJES_H */



