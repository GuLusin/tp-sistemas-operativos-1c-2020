#ifndef GAMEBOY_H
#define GAMEBOY_H


#include <utils/conexiones.h>
#include <utils/mensajes.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <string.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <pthread.h>
#include <semaphore.h>



typedef enum {
	BROKER,
	TEAM,
	GAMECARD,
	SUSCRIPTOR,
}tipo_proceso_gameboy;

t_log* logger;
t_config* config;

char *ip_broker,*puerto_broker;

int wait_time=3, socket_broker;


pthread_t pthread_recibir_mensajes_broker;

#endif /* CONEXIONES_H */


op_code interpretar_tipo_mensaje(char* tipo);
cola_code interpretar_cola_mensaje(char* tipo);
tipo_proceso_gameboy interpretar_tipo_proceso(char* tipo);
