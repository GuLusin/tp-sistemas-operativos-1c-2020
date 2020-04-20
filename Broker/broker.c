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

//gcc broker.c -lpthread -lcommons -o broker
//./broker

t_log* iniciar_logger(void)
{
	return log_create("team.log","log",1,LOG_LEVEL_INFO);
}

void inicializar_broker(){
	t_config* config;
    t_log* logger;
	int socket_broker;
	char * ip,*puerto;

	logger = iniciar_logger();

	if((config = config_create("config"))== NULL)
		perror("Error al crear la config");

	ip = config_get_string_value(config,"IP_BROKER");
	log_info(logger,config_get_string_value(config,"IP_BROKER")); //pido y logueo ip

    puerto = config_get_string_value(config,"PUERTO_BROKER");
	log_info(logger,config_get_string_value(config,"PUERTO_BROKER")); //pido y logueo puerto

	socket_broker = listen_to(ip,puerto);
	log_info(logger,"Socket: %d, escuchando",socket_broker);	//Socket queda escuchado

	recibir_cliente(socket_broker);
	log_info(logger,"Recibi al cliente");	//Recibi al cliente
}


int main(void) {
	t_log* logger; //creo log
	logger = iniciar_logger();
	log_info(logger,"Inicio conexion del broker");
	inicializar_broker();
	log_info(logger,"Termino conexion del broker");
	return EXIT_SUCCESS;
}
