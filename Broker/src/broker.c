/*                                                  HITO 2
 * Objetivos:
Proceso Team: Permitir solamente planificar de forma FIFO un conjunto de entrenadores.

Proceso Broker: Implementación completa de la administración de las colas de mensajes.
Aceptar suscripciones a una cola de mensajes específica.

Proceso GameBoy: Permitir el envío de varios mensajes al proceso Broker y el mensaje Appeared Pokemon
al proceso Team.
*/

#include "broker.h"

//#include<readline/readline.h>

//gcc broker.c -lpthread -lcommons -o broker
//./broker

t_log* iniciar_logger(void){
	t_log* log;
	if((log = log_create("broker.log","log",1,LOG_LEVEL_DEBUG)==NULL))
			perror("Error al crear log");
	return log;
}

t_config* iniciar_config(void){
	t_config* config;
	if((config = config_create("config"))== NULL)
		perror("Error al crear la config");
	return config;
}

void inicializar_broker(){
	//creo log
    t_log* logger;
    t_config* config;
	int socket_broker;
	char * ip,*puerto;
	config = iniciar_config();
	logger = iniciar_logger();


	ip = config_get_string_value(config,"IP_BROKER");
	log_debug(logger,config_get_string_value(config,"IP_BROKER")); //pido y logueo ip

    puerto = config_get_string_value(config,"PUERTO_BROKER");
	log_debug(logger,config_get_string_value(config,"PUERTO_BROKER")); //pido y logueo puerto

	socket_broker = listen_to(ip,puerto);
	log_debug(logger,"Socket: %d, escuchando",socket_broker);	//Socket queda escuchado

	recibir_cliente(socket_broker);
	log_debug(logger,"Recibi al cliente");	//Recibi al cliente
}


int main(void) {

	inicializar_broker();

	return EXIT_SUCCESS;
}
