/*                                                  HITO 2
 * Objetivos:
Proceso Team: Permitir solamente planificar de forma FIFO un conjunto de entrenadores.

Proceso Broker: Implementación completa de la administración de las colas de mensajes.
Aceptar suscripciones a una cola de mensajes específica.

Proceso GameBoy: Permitir el envío de varios mensajes al proceso Broker y el mensaje Appeared Pokemon
al proceso Team.
*/


//#include "/home/madd/eclipse-workspace/tp-2020-1c-cumpleDeFabian/utils/conexiones.c"

#include "../../utils/conexiones.c"
#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
//#include<readline/readline.h>


void inicializar_team(){
	t_config* config;
	char *ip,*puerto;
	int wait_time, socket_cola_localized,socket_cola_caught,socket_cola_appeared;

	if((config = config_create("config"))== NULL)
		perror("Error al crear la config");

	ip = config_get_string_value(config,"IP_BROKER");
    puerto = config_get_string_value(config,"PUERTO_BROKER");
    wait_time = config_get_int_value(config,"TIEMPO_RECONEXION");

	socket_cola_localized = connect_to(ip,puerto,wait_time);
	enviar_mensaje(socket_cola_localized, "localized");
	sleep(3);

	socket_cola_caught = connect_to(ip,puerto,wait_time);
	enviar_mensaje(socket_cola_caught, "caught");
	sleep(3);

	socket_cola_appeared = connect_to(ip,puerto,wait_time);
	enviar_mensaje(socket_cola_appeared, "appeared");
	sleep(3);
}

/*
 *Habria q hacerlo todo con funciones para conectar y enviar
 *Hacer un enum generico
 *En teoria habria q hacer un makefile donde esten todas las librerias a compilar
 *Q paso con la libreria que incluimos q tuvimos que poner .c en vez de .h?
 *Hacer un log para crear el "rastro" de las cosas realizadas
*/

int main(void) {
	puts("Inicializando");
	inicializar_team();
	puts("Termino");
	return EXIT_SUCCESS;
}
