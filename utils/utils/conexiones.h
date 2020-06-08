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
#include <commons/log.h>

#define ACK 777 // Puede ser cualquier valor elijo es pero es random
#define ID_CONFIRMATION 42069

pthread_t pthread;


uint32_t id_confirmation(int socket_aux);
bool send_ack(int socket_cliente, uint32_t ack);
bool check_ack(int socket_cliente, uint32_t value);
uint32_t wait_ack(int socket_cliente);
int sendall(int s, void *buf, int len);
int connect_to(char* ip_aux, char* puerto_aux,int wait_time,t_log* logger);
void esperar_cliente(int socket_servidor,void* funcion_recibir);
int listen_to(char* ip,char* puerto);


#endif /* CONEXIONES_H */
