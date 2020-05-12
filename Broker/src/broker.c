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

bool id_validation(int socket_cliente) {
	send_ack(socket_cliente, ACK); //Broker envia confirmacion del mensaje recibido
	if(check_ack(socket_cliente, ACK))// Broker espera al team para que confirme la confirmacion
		send_ack(socket_cliente, id_mensajes_globales++); // Broker le manda el id del mensaje
		return check_ack(socket_cliente, id_mensajes_globales - 1); //Broker espera la confirmacion del id y sale si es correcta.}

}
//#include<readline/readline.h>

//gcc broker.c -lpthread -lcommons -o broker
//./broker

void manejar_subscripcion(cola_code cola,int socket_cliente){

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
	puts("Mensaje recibido con exito!");
		id_validation(socket_cliente);
			return;

}

bool manejar_mensaje(t_mensaje* mensaje){
    switch(mensaje->codigo_operacion){
    	case GET_POKEMON:;
    		break;








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
	uint32_t codigo_operacion;

	if(recv(*socket_cliente, &(codigo_operacion),sizeof(uint32_t), 0)==-1){
		perror("Falla recv() op_code");
	}

	printf("op_code: %d\n", codigo_operacion);

	uint32_t id;

	if(recv(*socket_cliente, &(id), sizeof(uint32_t), 0) == -1){
		perror("Falla recv() id");
	}

	printf("id:%d\n", id);

	uint32_t size_contenido_mensaje;

	if(recv(*socket_cliente, &(size_contenido_mensaje), sizeof(uint32_t), 0) == -1){
		perror("Falla recv() size_contenido_mensaje");
	}


	printf("size contenido:%d\n", size_contenido_mensaje);

	if(codigo_operacion==SUBSCRIPCION){
		int cola;
		recv(*socket_cliente, &(cola),size_contenido_mensaje, 0);
		printf("cola:%d\n", cola);
		manejar_subscripcion(cola, *socket_cliente);
		//send_ack(socket_cliente);
		return;
	}

	void* stream = malloc(size_contenido_mensaje);

	if(recv(*socket_cliente, stream, size_contenido_mensaje, 0) == -1){
		perror("Falla recv() contenido");
	}

	t_mensaje* mensaje = deserializar_mensaje(codigo_operacion, stream);
	mensaje->id=id_mensajes_globales++;
	//send(*socket_cliente, mensaje->id, sizeof(uint32_t),0);
	manejar_mensaje(mensaje);


	/* if(manejar_mensaje(mensaje))
    	send_ack(*socket_cliente);
    //... y si no las hay, hacer free
*/

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

	id_mensajes_globales=0;

	pthread_t pthread_atender_cliente;
	pthread_mutex_init(&mutex_cola_new, NULL);
	pthread_mutex_init(&mutex_cola_get, NULL);
	pthread_mutex_init(&mutex_cola_catch, NULL);
	pthread_mutex_init(&mutex_cola_localized, NULL);
	pthread_mutex_init(&mutex_cola_caught, NULL);
	pthread_mutex_init(&mutex_cola_appeared, NULL);

	logger = log_create("broker.log", "log", true, LOG_LEVEL_DEBUG);
	config = config_create("../config");

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
	sleep(2);

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
