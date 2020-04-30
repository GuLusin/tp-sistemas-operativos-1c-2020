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


void recibir_mensaje(int *socket_cliente){

	printf("a recibir mensaje le llega el socket %d\n",*socket_cliente);
	int codigo_operacion;

	if(recv(*socket_cliente, &(codigo_operacion),sizeof(uint32_t), MSG_WAITALL)==-1){
		perror("Falla recv() op_code");
	}

	//if(cod=bus)

	int size;

	if(recv(*socket_cliente, &(size), sizeof(uint32_t), MSG_WAITALL) == -1){
		perror("Falla recv() buffer->size");
	}

	void* stream = malloc(size);

	if(recv(*socket_cliente, stream, size, MSG_WAITALL) == -1){
		perror("Falla recv() buffer->stream");
	}

	t_buffer* buffer= malloc(sizeof(t_buffer));
	buffer->size=size;
	buffer->stream=stream;
    deserializar_buffer(codigo_operacion,buffer,*socket_cliente);
}


/* recibir_cliente
 * socket_servidor = socket del cual se esperara la solicitud de conexion
 */

void recibir_cliente(int *socket_servidor){
	while(1){
		esperar_cliente(*socket_servidor,recibir_mensaje);
	}
}




void inicializar_broker(){

	int socket_broker;
	char *ip,*puerto;
	pthread_t pthreadew;

	logger = log_create("broker.log", "log", true, LOG_LEVEL_DEBUG);
	config = config_create("config");

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

	pthread_create(&pthreadew, NULL,(void*)recibir_cliente, &socket_broker);
	pthread_detach(pthreadew);
	log_debug(logger,"Recibi al cliente");	//Recibi al cliente

	getchar();
	close(socket_broker);






}


int main(void) {



	inicializar_broker();

	return EXIT_SUCCESS;
}
