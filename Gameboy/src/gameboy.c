//gcc broker.c -lpthread -lcommons -o broker
//./broker

#include "gameboy.h"

int subscribirse_a_cola(cola_code cola){
	int socket_broker = connect_to(ip_broker,puerto_broker,wait_time);
	void* stream = serializar_subscripcion(cola);
	sendall(socket_broker, stream, sizeof(uint32_t)*3);
	free(stream);
	//close(socket_broker);
	return socket_broker;
}

int interpretar_tipo_mensaje(char* tipo){
	if(!strcmp(tipo,"SUSCRIPTOR"))
		return SUSCRIPTOR;



}

int interpretar_cola_mensaje(char* tipo){
	if(!strcmp(tipo,"COLA_LOCALIZED_POKEMON"))
		return COLA_LOCALIZED_POKEMON;



}


void manejar_mensaje(int argc, char** args){
	puts("entra a manejar_mensaje");
	int tipo_mensaje = interpretar_tipo_mensaje(args[1]);
	switch(tipo_mensaje){
	case SUSCRIPTOR:
		if(argc!=4){
			perror("Cantidad de argumentos distinta de 3 para el tipo SUSCRIPTOR");

		}
		else{
			int cola = interpretar_cola_mensaje(args[2]);
			int tiempo = args[3];
			int socket_cliente = subscribirse_a_cola(cola);
			puts("ksuscrito a cola");
			//pthread_create(&pthread, NULL, recibir_mensaje, &socket_cliente);
			//pthread_detach(pthread);

		}

	}
}



void inicializar_gameboy(){

	logger = log_create("gameboy.log", "log", true, LOG_LEVEL_DEBUG);
	config = config_create("config");

	ip_broker = config_get_string_value(config,"IP_BROKER");
	log_debug(logger,ip_broker); //pido y logueo ip

    puerto_broker = config_get_string_value(config,"PUERTO_BROKER");
	log_debug(logger,puerto_broker); //pido y logueo puerto



}


int main(int argc, char**args) {
	if(argc ==1){
		puts("Nada");
		return EXIT_SUCCESS;
	}

	printf("argc=%d", COLA_LOCALIZED_POKEMON);

	inicializar_gameboy();

	manejar_mensaje(argc,args);


	return EXIT_SUCCESS;
}
