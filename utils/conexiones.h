/*
 * conexiones.h
 *
 *  Created on: Apr 20, 2020
 *      Author: madd
 */

#ifndef CONEXIONES_H
#define CONEXIONES_H

#include "mensajes.h"

#include <stdarg.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <pthread.h>

#define ACK 777 // Puede ser cualquier valor elijo es pero es random



typedef struct {
	int socket;
	int cola;
}t_subscripcion;

typedef struct{
	int id;
	int id_correlativo;
	int codigo_operacion;
	union{
		t_subscripcion subscripcion;
	}contenido;
}t_mensaje;

typedef struct{
	uint32_t size;
	void* stream;
} t_buffer;

typedef struct{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

pthread_t pthread;

void send_ack(int socket_cliente);
bool wait_ack(int socket_cliente);

int sendall(int s, void *buf, int len);

void esperar_cliente(int socket_servidor,void* funcion_recibir);
void* serializar_paquete(t_paquete* paquete, int tam_paquete);
void enviar_mensaje(int socket_a_enviar, char* mensaje);
int connect_to(char* ip, char* puerto,int wait_time);

t_mensaje* deserializar_buffer(int codigo_operacion, t_buffer* buffer,int socket_cliente);

void esperar_cliente(int socket_servidor,void* funcion_recibir);
int listen_to(char* ip,char* puerto);


#endif /* CONEXIONES_H */
