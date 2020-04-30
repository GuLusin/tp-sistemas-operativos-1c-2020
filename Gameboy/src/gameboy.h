#ifndef GAMEBOY_H
#define GAMEBOY_H


#include "../../utils/conexiones.c"
#include "../../utils/mensajes.c"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <commons/log.h>
#include <commons/config.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <pthread.h>



typedef enum {
	BROKER,
	TEAM,
	GAMECARD,
	SUSCRIPTOR,
}tipo_mensaje_gameboy;

t_log* logger;
t_config* config;

char *ip_broker,*puerto_broker;

int wait_time=3, socket_broker;


pthread_t thread;

#endif /* CONEXIONES_H */
