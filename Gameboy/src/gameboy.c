//gcc broker.c -lpthread -lcommons -o broker
//./broker

#include "gameboy.h"

int subscribirse_a_cola(cola_code cola){
	int socket_aux = connect_to(ip_broker,puerto_broker,wait_time);
	t_mensaje* mensaje = crear_mensaje(2, SUBSCRIPCION, cola);
	mensaje->id=ID_SUSCRIPCION;
	//printf("op_code:%d\nid:%d\ncola contenido:%d\n", mensaje->codigo_operacion,mensaje->id,mensaje->contenido.subscripcion);
	enviar_mensaje(socket_aux, mensaje);
	printf("espera id:%d",cola);
	check_ack(socket_aux, ACK);
	return socket_aux;
}

void escuchar_broker(int *socket_servidor){
	while(true){
		//puts("escuchar_broker");
		uint32_t codigo_operacion;

		if(recv(*socket_servidor, &(codigo_operacion),sizeof(uint32_t), 0)==-1){
			perror("Falla recv() op_code");
		}

		printf("op_code: %d\n", codigo_operacion);

		uint32_t id;

		if(recv(*socket_servidor, &(id), sizeof(uint32_t), 0) == -1){
			perror("Falla recv() id");
		}

		printf("id:%d\n", id);

		uint32_t size_contenido_mensaje;

		if(recv(*socket_servidor, &(size_contenido_mensaje), sizeof(uint32_t), 0) == -1){
			perror("Falla recv() size_contenido_mensaje");
		}


		printf("size contenido:%d\n", size_contenido_mensaje);

		void* stream = malloc(size_contenido_mensaje);

		if(recv(*socket_servidor, stream, size_contenido_mensaje, 0) == -1){
			perror("Falla recv() contenido");
		}

		t_mensaje* mensaje = deserializar_mensaje(codigo_operacion, stream);
	}
}

void subscripcion_temporal_a_cola(cola_code cola, int wait_time){
	int socket_broker = subscribirse_a_cola(cola);
	printf("subscrito a cola:%d",cola);

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

cola_code interpretar_cola_mensaje(char* cola){
	if(!strcmp(cola,"COLA_LOCALIZED_POKEMON"))
		return COLA_LOCALIZED_POKEMON;
	if(!strcmp(cola,"COLA_GET_POKEMON"))
		return COLA_GET_POKEMON;
	if(!strcmp(cola,"COLA_NEW_POKEMON"))
		return COLA_NEW_POKEMON;
	if(!strcmp(cola,"COLA_APPEARED_POKEMON"))
		return COLA_APPEARED_POKEMON;
	if(!strcmp(cola,"COLA_CAUGHT_POKEMON"))
		return COLA_CAUGHT_POKEMON;
	if(!strcmp(cola,"COLA_CATCH_POKEMON"))
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
				mensaje = crear_mensaje(4,args[3],args[4],args[5],args[6]);
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
		cola_code cola = interpretar_cola_mensaje(args[2]);
		printf("cola:%d", cola);
		int wait_time = atoi(args[3]);
		printf("wait_time:%d", wait_time);
		subscripcion_temporal_a_cola(cola, wait_time);
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
