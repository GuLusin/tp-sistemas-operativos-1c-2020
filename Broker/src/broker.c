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

void manejar_subscripcion(){

}

t_config* iniciar_config(void){
	t_config* config;
	if((config = config_create("config")) == 0)
		perror("Error al crear la config");
	return config;
}

void inicializar_broker(){

	int socket_broker;
	char *ip,*puerto;
	pthread_t pthread;

	logger = log_create("broker.log", "log", true, LOG_LEVEL_DEBUG);
	config = iniciar_config();

	sockets_cola_new = list_create();
	sockets_cola_get = list_create();
	sockets_cola_catch = list_create();
	sockets_cola_localized = list_create();
	sockets_cola_caught = list_create();
	sockets_cola_appeared = list_create();

	ip = config_get_string_value(config,"IP_BROKER");
	log_debug(logger,ip); //pido y logueo ip

    puerto = config_get_string_value(config,"PUERTO_BROKER");
	log_debug(logger,puerto); //pido y logueo puerto

	socket_broker = listen_to(ip,puerto);
	log_debug(logger,"Socket: %d, escuchando",socket_broker);	//Socket queda escuchado

	//.............................

	puts("Esto se esta por descontrolar:");

	sleep(5);

	pthread_create(&pthread, NULL,(void*)recibir_cliente, &socket_broker);
	pthread_detach(pthread);
	log_debug(logger,"Recibi al cliente");	//Recibi al cliente

	getchar();
	close(socket_broker);






}


int main(void) {



	inicializar_broker();

	return EXIT_SUCCESS;
}
