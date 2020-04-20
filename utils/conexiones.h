
/*
 * ARCHIVO SOCKETS
 */

#ifndef SOCKET_H_
#define SOCKET_H_

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
	MENSAJE = 1,
}op_code;

typedef struct{
	uint32_t size;
	void* stream;
} t_buffer;

typedef struct{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

pthread_t pthread;

void esperar_cliente(int socket_servidor);
void* serializar_paquete(t_paquete* paquete, int tam_paquete);
void enviar_mensaje(int socket_cliente, char* mensaje);
int connect_to(char* ip, char* puerto,int wait_time);
void recibir_cliente(int socket_servidor);
void deserializar_buffer(int codigo_operacion, t_buffer* buffer);
void recibir_mensaje(int *socket_cliente);
void esperar_cliente(int socket_servidor);
int listen_to(char* ip,char* puerto);


#endif /* SOCKET_H_ */
