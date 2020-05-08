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

typedef struct {
	int socket;
	int cola;
}t_subscripcion;

typedef struct{
	int id;
//	int id_correlativo;
	int codigo_operacion;
	union{
		t_subscripcion subscripcion; //para subscripcion
	}contenido;
}t_mensaje;

typedef enum
{
	STRING,
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


void* serializar_subscripcion(cola_code cola);
cola_code deserializar_subscripcion(void* stream);
t_mensaje* deserializar_buffer(int codigo_operacion, t_buffer* buffer,int socket_cliente);
t_mensaje* crear_mensaje(int argc, ...);


#endif /* MENSAJES_H */
