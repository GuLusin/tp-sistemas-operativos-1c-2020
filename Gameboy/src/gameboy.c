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

op_code interpretar_tipo_mensaje(char* tipo){
	if(!strcmp(tipo,"APPEARED_POKEMON"))
		return APPEARED_POKEMON;
	if(!strcmp(tipo,"NEW_POKEMON"))
		return NEW_POKEMON;
	if(!strcmp(tipo,"CATCH_POKEMON"))
		return CATCH_POKEMON;
	if(!strcmp(tipo,"CAUGHT_POKEMON"))
		return CAUGHT_POKEMON;
	if(!strcmp(tipo,"GET_POKEMON"))
		return GET_POKEMON;
}

tipo_proceso_gameboy interpretar_tipo_proceso(char* proceso){
	if(!strcmp(proceso,"BROKER"))
		return BROKER;
	if(!strcmp(proceso,"GAMECARD"))
		return GAMECARD;
	if(!strcmp(proceso,"TEAM"))
		return TEAM;
	if(!strcmp(proceso,"SUSCRIPTOR"))
		return SUSCRIPTOR;
}

cola_code interpretar_cola_mensaje(char* tipo){
	if(!strcmp(tipo,"COLA_LOCALIZED_POKEMON"))
		return COLA_LOCALIZED_POKEMON;
	if(!strcmp(tipo,"COLA_GET_POKEMON"))
		return COLA_GET_POKEMON;
	if(!strcmp(tipo,"COLA_NEW_POKEMON"))
		return COLA_NEW_POKEMON;
	if(!strcmp(tipo,"COLA_APPEARED_POKEMON"))
		return COLA_APPEARED_POKEMON;
	if(!strcmp(tipo,"COLA_CAUGHT_POKEMON"))
		return COLA_CAUGHT_POKEMON;
	if(!strcmp(tipo,"COLA_CATCH_POKEMON"))
		return COLA_CATCH_POKEMON;
}


void manejar_mensaje(int argc, char** args){
	puts("entra a manejar_mensaje");
	tipo_proceso_gameboy proceso = interpretar_tipo_proceso(args[1]);
	op_code tipo_mensaje = interpretar_tipo_mensaje(args[2]);
	t_mensaje* mensaje;
	switch(proceso){
	case BROKER:
		switch(tipo_mensaje){
		case NEW_POKEMON:;
				mensaje = crear_mensaje(argc,args[3],args[4],args[5],args[6]);
				socket_broker = connect_to(ip_broker, puerto_broker, wait_time);
				enviar_mensaje(socket_broker, mensaje);
				break;

		case APPEARED_POKEMON:;

			break;
		case CATCH_POKEMON:;

			break;
		case CAUGHT_POKEMON:;

			break;
		case GET_POKEMON:;
			break;

		}

	case TEAM:;
		break;
	case GAMECARD:;
		break;
	case SUSCRIPTOR:;
		break;
	}
}



void inicializar_gameboy(){

	logger = log_create("gameboy.log", "log", true, LOG_LEVEL_DEBUG);
	config = config_create("../config");

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

	inicializar_gameboy();

	manejar_mensaje(argc,args);


	return EXIT_SUCCESS;
}
