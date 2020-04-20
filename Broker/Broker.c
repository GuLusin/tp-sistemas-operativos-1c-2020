/*                                                  HITO 2
 * Objetivos:
Proceso Team: Permitir solamente planificar de forma FIFO un conjunto de entrenadores.

Proceso Broker: Implementación completa de la administración de las colas de mensajes.
Aceptar suscripciones a una cola de mensajes específica.

Proceso GameBoy: Permitir el envío de varios mensajes al proceso Broker y el mensaje Appeared Pokemon
al proceso Team.
*/

#include "/home/utnso/workspace/tp-2020-1c-cumpleDeFabian/utils/conexiones.c"
#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
//#include<readline/readline.h>


void inicializar_broker(){
	int socket_broker;
	t_config* config;
	char * ip,*puerto;
	if((config = config_create("config"))== NULL)
		perror("Error al crear la config");

	ip = config_get_string_value(config,"IP_BROKER");
    puerto = config_get_string_value(config,"PUERTO_BROKER");

	socket_broker = listen_to(ip,puerto);
	recibir_cliente(socket_broker);
}


int main(void) {
	puts("Inicializando");
	inicializar_broker();
	puts("Termino");
	return EXIT_SUCCESS;
}
