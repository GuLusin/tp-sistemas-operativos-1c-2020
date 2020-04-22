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

typedef enum
{
	COLA_GET_POKEMON,
	COLA_CATCH_POKEMON,
	COLA_LOCALIZED_POKEMON,
	COLA_CAUGHT_POKEMON,
	COLA_APPEARED_POKEMON,
} cola_code;


typedef enum
{
	STRING,
	SUSCRIPCION,
	LOCALIZED_POKEMON,
	CAUGHT_POKEMON,
	APPEARED_POKEMON,
} op_code;

pthread_t pthread;


#endif /* MENSAJES_H */
