/*                                                  HITO 2
 * Objetivos:
Proceso Team: Permitir solamente planificar de forma FIFO un conjunto de entrenadores.

Proceso Broker: Implementación completa de la administración de las colas de mensajes.
Aceptar suscripciones a una cola de mensajes específica.

Proceso GameBoy: Permitir el envío de varios mensajes al proceso Broker y el mensaje Appeared Pokemon
al proceso Team.

    delimitador[] = ",";
    string_split(cadena, delimitador);

*/

#include "broker.h"

//#include<readline/readline.h>

//gcc broker.c -lpthread -lcommons -o broker
//./broker

void manejar_subscripcion(t_mensaje* mensaje){
	cola_code cola = mensaje->contenido.subscripcion.cola;
	int socket_cliente  = mensaje->contenido.subscripcion.socket;

	switch(cola){
		case COLA_APPEARED_POKEMON:
			pthread_mutex_lock(&mutex_cola_appeared);
			list_add(sockets_cola_appeared, (void*)socket_cliente);
			pthread_mutex_unlock(&mutex_cola_appeared);
			break;
		case COLA_CAUGHT_POKEMON:
			pthread_mutex_lock(&mutex_cola_caught);
			list_add(sockets_cola_caught, (void*)socket_cliente);
			pthread_mutex_unlock(&mutex_cola_caught);
			break;
		case COLA_LOCALIZED_POKEMON:
			pthread_mutex_lock(&mutex_cola_localized);
			list_add(sockets_cola_localized, (void*)socket_cliente);
			pthread_mutex_unlock(&mutex_cola_localized);
			break;
		case COLA_CATCH_POKEMON:
			pthread_mutex_lock(&mutex_cola_catch);
			list_add(sockets_cola_catch, (void*)socket_cliente);
			pthread_mutex_unlock(&mutex_cola_catch);
			break;
		case COLA_GET_POKEMON:
			pthread_mutex_lock(&mutex_cola_get);
			list_add(sockets_cola_get, (void*)socket_cliente);
			pthread_mutex_unlock(&mutex_cola_get);
			break;
		case COLA_NEW_POKEMON:
			pthread_mutex_lock(&mutex_cola_new);
			list_add(sockets_cola_new, (void*)socket_cliente);
			pthread_mutex_unlock(&mutex_cola_new);
			break;
	}
	puts("mensaje recibido con exito!");
}

bool manejar_mensaje(t_mensaje* mensaje){
    switch(mensaje->codigo_operacion){
    	case SUBSCRIPCION:
    		manejar_subscripcion(mensaje);
    		return true;



    }
}


/* recibir_mensaje
 * socket_cliente = socket del cual hace recv para obtener el mensaje que llega.
 *
 * Primero delega a serializar buffer el cual devuelve un t_mensaje* el cual luego es pasado a
 * manejar mensaje que se encarga de procesarlo.
 */



void recibir_mensaje(int *socket_cliente){
	//printf("a recibir mensaje le llega el socket %d\n",*socket_cliente);
	int codigo_operacion;

	if(recv(*socket_cliente, &(codigo_operacion),sizeof(uint32_t), MSG_WAITALL)==-1){
		perror("Falla recv() op_code");
	}

	//Aca se implementaria deserializar_buffer para alternativa cod/buffer

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
    t_mensaje* mensaje = deserializar_buffer(codigo_operacion,buffer,*socket_cliente);
    if(manejar_mensaje(mensaje))
    	send_ack(*socket_cliente);
    //... y si no las hay, hacer free


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

	pthread_t pthread_atender_cliente;
	pthread_mutex_init(&mutex_cola_new, NULL);
	pthread_mutex_init(&mutex_cola_get, NULL);
	pthread_mutex_init(&mutex_cola_catch, NULL);
	pthread_mutex_init(&mutex_cola_localized, NULL);
	pthread_mutex_init(&mutex_cola_caught, NULL);
	pthread_mutex_init(&mutex_cola_appeared, NULL);

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

	//revisar esto:
	if((socket_broker = listen_to(ip,puerto)) == -1)
		return;

	log_debug(logger,"Socket: %d, escuchando",socket_broker);	//Socket queda escuchado

	//.............................

	puts("Espera de 5s antes de comenzar a recibir sockets:\n");
	sleep(5);

	pthread_create(&pthread_atender_cliente, NULL,(void*)recibir_cliente, &socket_broker);
	pthread_detach(pthread_atender_cliente);
	log_debug(logger,"Recibi al cliente");	//Recibi al cliente

	getchar();

	printf("cola appeared: %d\ncola caught: %d\ncola localized: %d\n", (int)list_get(sockets_cola_appeared,0),(int)list_get(sockets_cola_caught,0),(int)list_get(sockets_cola_localized,0));
	close(socket_broker);

}


int main(void) {



	inicializar_broker();

	return EXIT_SUCCESS;
}
