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

typedef enum {
	COLA_NEW_POKEMON,
	COLA_GET_POKEMON,
	COLA_CATCH_POKEMON,
	COLA_LOCALIZED_POKEMON,
	COLA_CAUGHT_POKEMON,
	COLA_APPEARED_POKEMON,
} cola_code;


typedef struct{
	uint32_t size_pokemon;
	char* pokemon;
}t_get_pokemon;

typedef struct{
	uint32_t id;
	uint32_t codigo_operacion;
	union{
		uint32_t subscripcion;
		t_get_pokemon* get_pokemon;
	}contenido;
}t_mensaje;

//size se envia en la serializacion aunque no este en el struct

typedef enum
{
	SUBSCRIPCION,
	NEW_POKEMON,
	LOCALIZED_POKEMON,
	CAUGHT_POKEMON,
	APPEARED_POKEMON,
	GET_POKEMON,
	CATCH_POKEMON,
} op_code;

typedef struct{
	uint32_t size;
	void* stream;
} t_buffer;

typedef struct{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;


void enviar_mensaje(int socket_a_enviar, t_mensaje* mensaje);
void* serializar_paquete(t_paquete* paquete, int tam_paquete);
void* serializar_subscripcion(cola_code cola);
cola_code deserializar_subscripcion(void* stream);
t_mensaje* deserializar_mensaje(int codigo_operacion, void* stream);
t_mensaje* crear_mensaje(int argc, ...);


#endif /* MENSAJES_H */
