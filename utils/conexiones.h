/*
 * conexiones.h
 *
 *  Created on: Apr 20, 2020
 *      Author: madd
 */

#ifndef CONEXIONES_H
#define CONEXIONES_H

#include "mensajes.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <pthread.h>


typedef struct{
	uint32_t size;
	void* stream;
} t_buffer;

typedef struct{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

pthread_t pthread;

void esperar_cliente(int socket_servidor,void* funcion_recibir);
void* serializar_paquete(t_paquete* paquete, int tam_paquete);
void enviar_mensaje(int socket_a_enviar, char* mensaje);
int connect_to(char* ip, char* puerto,int wait_time);

void deserializar_buffer(int codigo_operacion, t_buffer* buffer,int socket_cliente);

void esperar_cliente(int socket_servidor,void* funcion_recibir);
int listen_to(char* ip,char* puerto);


#endif /* CONEXIONES_H */
