#include "gamecard.h"

int subscribirse_a_cola(cola_code cola){
	int socket_aux = connect_to(ip_broker,puerto_broker, tiempo_reintento_conexion);
	t_mensaje* mensaje = crear_mensaje(2, SUBSCRIPCION, cola);
	mensaje->id=ID_SUSCRIPCION;
	enviar_mensaje(socket_aux, mensaje);
	check_ack(socket_aux, ACK);
	return socket_aux;
}

void leer_config() {
	config = config_create("../config");
	tiempo_reintento_conexion = config_get_int_value(config,"TIEMPO_DE_REINTENTO_CONEXION");
	tiempo_reintento_operacion = config_get_int_value(config,"TIEMPO_DE_REINTENTO_OPERACION");
	ip_broker = config_get_string_value(config,"IP_BROKER");
	log_debug(logger,config_get_string_value(config,"IP_BROKER")); //pido y logueo ip
	puerto_broker = config_get_string_value(config,"PUERTO_BROKER");
	log_debug(logger,config_get_string_value(config,"PUERTO_BROKER")); //pido y logueo puerto
	punto_montaje = config_get_string_value(config,"PUNTO_MONTAJE_TALLGRASS");
	//log_debug(logger,config_get_string_value(config,"PUNTO_MONTAJE_TALLGRASS")); // Ver si es necesario loggear esto
}


/*

t_mensaje* recibir_mensaje(){
	uint32_t codigo_operacion=CODIGO_OPERACION_DEFAULT;


	if(recv(*socket_broker, &(codigo_operacion),sizeof(uint32_t), 0)==-1){
		perror("Falla recv() op_code");
		return 0;
	}

	printf("op_code:%d\n", codigo_operacion);

	if(codigo_operacion==CODIGO_OPERACION_DEFAULT){
		return 0;
	}
	uint32_t id;

	if(recv(*socket_broker, &(id), sizeof(uint32_t), 0) == -1){
		perror("Falla recv() id");
		return 0;
	}

	printf("id:%d\n", id);

	uint32_t size_contenido_mensaje;

	if(recv(*socket_broker, &(size_contenido_mensaje), sizeof(uint32_t), 0) == -1){
		perror("Falla recv() size_contenido_mensaje");
		return 0;
	}

	printf("size contenido:%d\n", size_contenido_mensaje);


	void* stream = malloc(size_contenido_mensaje);

	if(recv(*socket_broker, stream, size_contenido_mensaje, 0) == -1){
		perror("Falla recv() contenido");
		return 0;
	}

	t_mensaje* mensaje = deserializar_mensaje(codigo_operacion, stream);
	mensaje->id=id;
	return mensaje;
}


*/

void escuchar_cola_new(){
	while(1){
		printf("new \n");
		//escucha el socket de la cola new y si llega algo
		//crearia el hilo que atiende la peticion
	}
}

void escuchar_cola_catch(){
	while(1){
		printf("catch \n");
	}
}

void escuchar_cola_get(){
	while(1){
		printf("get \n");

	}
}

void revisar_conexion_broker(){
	while(1){
		//estas ahi broker weon?
		printf("brokeeer \n");
	}
}

void inicializar_gamecard() { // Suscribirse a NEW_POKEMON, CATCH_POKEMON, GET_POKEMON
	logger = log_create("../gamecard.log","log",1,LOG_LEVEL_DEBUG);
	leer_config();

	pthread_t pthread_cola_new;
	pthread_t pthread_cola_catch;
	pthread_t pthread_cola_get;
	pthread_t pthread_conexion_broker;

	socket_cola_get = subscribirse_a_cola(COLA_GET_POKEMON);
	socket_cola_catch = subscribirse_a_cola(COLA_CATCH_POKEMON);
	socket_cola_new = subscribirse_a_cola(COLA_NEW_POKEMON);

	pthread_create(&pthread_conexion_broker, NULL, (void*)revisar_conexion_broker,NULL);
	pthread_detach(pthread_conexion_broker);

	pthread_create(&pthread_cola_new, NULL,(void*)escuchar_cola_new, NULL);
	pthread_detach(pthread_cola_new);


	pthread_create(&pthread_cola_catch, NULL,(void*)escuchar_cola_catch, NULL);
	pthread_detach(pthread_cola_catch);

	pthread_create(&pthread_cola_get, NULL,(void*)escuchar_cola_get, NULL);
	pthread_detach(pthread_cola_get);
}

void cerrar_gamecard(){
	log_destroy(logger);
	config_destroy(config);
	close(socket_cola_new);
}

int main(void) {
	inicializar_gamecard();
	cerrar_gamecard();
	return EXIT_SUCCESS;
}
