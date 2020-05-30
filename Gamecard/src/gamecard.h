#ifndef GAMECARD_H
#define GAMECARD_H

#include <utils/conexiones.h>
#include <utils/mensajes.h>
#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <pthread.h>
#include <semaphore.h>
#include <commons/collections/list.h>

t_config* config;
t_log* logger;
int tiempo_reintento_conexion;
int tiempo_reintento_operacion;
char *ip_broker, *puerto_broker, *punto_montaje;
int socket_broker, socket_cola_new, socket_cola_catch, socket_cola_get;

#endif /* GAMECARD_H*/
