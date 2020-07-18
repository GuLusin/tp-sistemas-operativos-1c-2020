
#include "gameboy.h"

void subscribirse_a_cola(cola_code cola){//todo recibir mensajes y hacer ACK por cada mensaje.
	char *ip_send = config_get_string_value(config,"IP_BROKER");
	char *puerto_send = config_get_string_value(config,"PUERTO_BROKER");
	int socket_aux = connect_to(ip_send,puerto_send,wait_time,logger);
	t_mensaje* mensaje = crear_mensaje(2, SUBSCRIPCION, cola);
	mensaje->id=ID_SUSCRIPCION;
	enviar_mensaje(socket_aux, mensaje);
	check_ack(socket_aux,ACK);
	log_debug(logger,"Subscrito a cola %d",cola);
	recibir_broker(&socket_aux);
}


void recibir_broker(int *socket_servidor){
	while(true){
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

		send_ack(*socket_servidor,ACK);

		free(stream);
	}
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
	perror("Falla interpretar_tipo_mensaje");
	return -1;
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
	perror("Falla interpretar_tipo_proceso");
	return -1;
}

cola_code interpretar_cola_mensaje(char* cola){
	if(!strcmp(cola,"LOCALIZED_POKEMON"))
		return COLA_LOCALIZED_POKEMON;
	if(!strcmp(cola,"GET_POKEMON"))
		return COLA_GET_POKEMON;
	if(!strcmp(cola,"NEW_POKEMON"))
		return COLA_NEW_POKEMON;
	if(!strcmp(cola,"APPEARED_POKEMON"))
		return COLA_APPEARED_POKEMON;
	if(!strcmp(cola,"CAUGHT_POKEMON"))
		return COLA_CAUGHT_POKEMON;
	if(!strcmp(cola,"CATCH_POKEMON"))
		return COLA_CATCH_POKEMON;
	perror("Falla interpretar_cola_mensaje");
	return -1;
}

void sleep_hasta(int* tiempo){
	sleep(*tiempo);
	int pid = process_getpid();
	close(socket_aux);
	kill(pid,SIGKILL);
}


void manejar_mensaje(int argc, char** args){

	char *ip_send, *puerto_send;

	tipo_proceso_gameboy proceso = interpretar_tipo_proceso(args[1]);
	op_code tipo_mensaje = interpretar_tipo_mensaje(args[2]);

	t_mensaje* mensaje;

	switch(proceso){

	case BROKER:
		ip_send = config_get_string_value(config,"IP_BROKER");
	    puerto_send = config_get_string_value(config,"PUERTO_BROKER");
		socket_aux = connect_to(ip_send,puerto_send,wait_time,logger);
		log_debug(logger,"Conectado a proceso BROKER ip:%s	puerto:%s",ip_send,puerto_send);

		switch(tipo_mensaje){
		case NEW_POKEMON:
			mensaje = crear_mensaje(5,NEW_POKEMON,args[3],atoi(args[4]),atoi(args[5]),atoi(args[6]));
			break;
		case APPEARED_POKEMON:
			mensaje = crear_mensaje(5,APPEARED_POKEMON,args[3],atoi(args[4]),atoi(args[5]),atoi(args[6]));
			break;
		case CATCH_POKEMON:
			mensaje = crear_mensaje(4,CATCH_POKEMON,args[3],atoi(args[4]),atoi(args[5]));
			printear_mensaje(mensaje);
			enviar_mensaje(socket_aux,mensaje);
			check_ack(socket_aux,ACK);
			check_ack(socket_aux,ACK);
			printf("ACK recibido con exito\n");
			free(mensaje);
			close(socket_aux);
			printf("Socket cerrado\n");
			return;
			break;
		case CAUGHT_POKEMON:
			if(!strcmp(strdup(args[4]),"OK")){
				mensaje = crear_mensaje(3,CAUGHT_POKEMON,atoi(args[3]),1);
				break;
			}
			if(!strcmp(strdup(args[4]),"FAIL")){
				mensaje = crear_mensaje(3,CAUGHT_POKEMON,atoi(args[3]),0);
				break;
			}
			printf("Mensaje invalido\n");
			return;
		case GET_POKEMON:
			mensaje = crear_mensaje(2,GET_POKEMON,args[3]);
			printear_mensaje(mensaje);
			enviar_mensaje(socket_aux,mensaje);
			check_ack(socket_aux,ACK);
			check_ack(socket_aux,ACK);
			printf("ACK recibido con exito\n");
			free(mensaje);
			close(socket_aux);
			printf("Socket cerrado\n");
			return;
			break;
		default:
			printf("Mensaje invalido\n");
			return;
		}
		break;

	case TEAM:
		ip_send = config_get_string_value(config,"IP_TEAM");
		puerto_send = config_get_string_value(config,"PUERTO_TEAM");
		socket_aux = connect_to(ip_send,puerto_send,wait_time,logger);
		log_debug(logger,"Conectado a proceso TEAM ip:%s	puerto:%s",ip_send,puerto_send);

		if(tipo_mensaje == APPEARED_POKEMON){
			mensaje = crear_mensaje(5,APPEARED_POKEMON,args[3],atoi(args[4]),atoi(args[5]),ID_DEFAULT);
			break;
		}
		printf("Mensaje invalido\n");
		return;

	case GAMECARD:
		ip_send = config_get_string_value(config,"IP_GAMECARD");
	    puerto_send = config_get_string_value(config,"PUERTO_GAMECARD");
		socket_aux = connect_to(ip_send,puerto_send,wait_time,logger);
		log_debug(logger,"Conectado a proceso GAMECARD ip:%s	puerto:%s",ip_send,puerto_send);

		switch(tipo_mensaje){
		case NEW_POKEMON:
			mensaje = crear_mensaje(5,NEW_POKEMON,args[3],atoi(args[4]),atoi(args[5]),atoi(args[6]));
			mensaje->id=atoi(args[7]);
			break;
		case CATCH_POKEMON:
			mensaje = crear_mensaje(4,CATCH_POKEMON,args[3],atoi(args[4]),atoi(args[5]));
			mensaje->id=atoi(args[6]);
			break;
		case GET_POKEMON:
			mensaje = crear_mensaje(2,GET_POKEMON,args[3]); // ./Gameboy GAMECARD GET_POKEMON Squirtle 2
			mensaje->id=atoi(args[4]);
			printf("id mensaje:%d\n",mensaje->id);
			break;
		default:
			printf("Mensaje invalido\n");
			return;
		}
		break;

	case SUSCRIPTOR:;

		int time = atoi(args[3]);
		printf("tiempo:%d\n", time);
		pthread_create(&pthread_mensajes_broker,NULL, sleep_hasta,&time);//crear un thread que le paso el tiempo y el socket y luego de ese tiempo cierra el socket y joinea
		pthread_detach(pthread_mensajes_broker);
		subscribirse_a_cola(interpretar_cola_mensaje(args[2]));
		puts("Asd");
		return;
	}

	printear_mensaje(mensaje);
	enviar_mensaje(socket_aux,mensaje);
	check_ack(socket_aux,ACK);
	printf("ACK recibido con exito\n");
	free(mensaje);
	close(socket_aux);
	printf("Socket cerrado\n");
	return;
}

int main(int argc, char**args) {

	if(argc == 1){
		puts("Se tiene que ejecutar gameboy con los argumentos a usar");
		return EXIT_SUCCESS;
	}

	logger = log_create("gameboy.log", "log", true, LOG_LEVEL_DEBUG);
	config = config_create("../config");

	manejar_mensaje(argc,args);

	return EXIT_SUCCESS;
}
