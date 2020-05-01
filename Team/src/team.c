/*                                                  HITO 2
 * Objetivos:
Proceso Team: Permitir solamente planificar de forma FIFO un conjunto de entrenadores.

Proceso Broker: Implementación completa de la administración de las colas de mensajes.
Aceptar suscripciones a una cola de mensajes específica.

Proceso GameBoy: Permitir el envío de varios mensajes al proceso Broker y el mensaje Appeared Pokemon
al proceso Team.
*/

/*
 * team.c: In function ‘iniciar_logger’:
team.c:21:10: warning: assignment makes pointer from integer without a cast [-Wint-conversion]
  if((log = log_create("broker.log","log",1,LOG_LEVEL_DEBUG)==NULL))
          ^
team.c: In function ‘subscribirse_a_colas’:
team.c:46:17: warning: implicit declaration of function ‘serializar_subscripcion’ [-Wimplicit-function-declaration]
  void* stream = serializar_subscripcion(2);
                 ^~~~~~~~~~~~~~~~~~~~~~~
team.c:46:17: warning: initialization makes pointer from integer without a cast [-Wint-conversion]
/tmp/ccdsswnv.o: In function `subscribirse_a_colas':
team.c:(.text+0x705): undefined reference to `serializar_subscripcion'
collect2: error: ld returned 1 exit status
 *
 */



#include "team.h"

//#include<readline/readline.h>

//gcc team.c -lpthread -lcommons -o team
//./team



int subscribirse_a_cola(cola_code cola){
	int socket_broker = connect_to(ip_broker,puerto_broker,wait_time);
	void* stream = serializar_subscripcion(cola);
	sendall(socket_broker, stream, sizeof(uint32_t)*3);
	free(stream);
	if(wait_ack(socket_broker)){
		puts("recibio acknowlegment del mensaje");
	}
	//close(socket_broker);
	return socket_broker;
}

void subscribirse_a_colas(){

	int socket_broker, socket_cola_localized, socket_cola_caught, socket_cola_appeared;

	//Obtiene los datos IP,PUERTO WAIT_TIME desde la config

	ip_broker = config_get_string_value(config,"IP_BROKER");
	log_debug(logger,config_get_string_value(config,"IP_BROKER")); //pido y logueo ip
	puerto_broker = config_get_string_value(config,"PUERTO_BROKER");
	log_debug(logger,config_get_string_value(config,"PUERTO_BROKER")); //pido y logueo puerto
	wait_time = config_get_int_value(config,"TIEMPO_RECONEXION");

	// Intenta conexion con el broker y envia la serializacion
/*
	socket_broker = connect_to(ip_broker,puerto_broker,wait_time);

	void* stream = serializar_subscripcion(COLA_APPEARED_POKEMON);


	send(socket_broker, stream, sizeof(uint32_t)*3, 0);
	free(stream);
	//close(socket_broker);

	sleep(3);
	socket_broker = connect_to(ip_broker,puerto_broker,wait_time);
	stream = serializar_subscripcion(COLA_LOCALIZED_POKEMON);
	send(socket_broker, stream, sizeof(uint32_t)*3, 0);
	free(stream);
	//close(socket_broker);

	sleep(3);
	socket_broker = connect_to(ip_broker,puerto_broker, wait_time);
	stream = serializar_subscripcion(COLA_CAUGHT_POKEMON);
	send(socket_broker, stream, sizeof(uint32_t)*3, 0);
	free(stream);
	//close(socket_broker);

	//se subscribe a COLA_GET_POKEMON y sale

/**/
	//int a=100;
	subscribirse_a_cola(COLA_APPEARED_POKEMON);
	//usleep(a);
	subscribirse_a_cola(COLA_LOCALIZED_POKEMON);
	//usleep(a);
	subscribirse_a_cola(COLA_CAUGHT_POKEMON);


}

void inicializar_team(){

	puts ("lala");
	t_list *entrenadores;
	char **lista_de_objetivos, **lista_de_pokemones, **lista_de_posiciones;
	int cantidad_entrenadores;
	entrenador entrenador;

	lista_de_posiciones = config_get_array_value(config,"POSICIONES_ENTRENADORES");

	printf("%s",lista_de_posiciones[0]);

	lista_de_pokemones = config_get_array_value(config,"POKEMON_ENTRENADORES");

	printf("%s",lista_de_pokemones[0]);

	lista_de_objetivos = config_get_array_value(config,"OBJETIVOS_ENTRENADORES");

	printf("%s",lista_de_objetivos[0]);

	cantidad_entrenadores = list_size(lista_de_posiciones);


	for(int i = 0; i < cantidad_entrenadores; i++){
		int *x, *y;
		char **auxiliar = string_split(lista_de_posiciones[i],"|");

		entrenador.posicion_x = (int)auxiliar[0];
		entrenador.posicion_y = (int)auxiliar[1];

		auxiliar = string_split(lista_de_pokemones[i],"|"); //ultima posicion tiene null

		entrenador.pokemones = list_create();
		while(*auxiliar){
		  list_add(entrenador.pokemones,(void *) *auxiliar);
	      auxiliar++; //sigue hasta encontrar null
		}

		auxiliar = string_split(lista_de_objetivos[i],"|"); //ultima posicion tiene null
		entrenador.objetivos= list_create();
		while(*auxiliar){
		  list_add(entrenador.objetivos,(void *) *auxiliar);
		  auxiliar++; //sigue hasta encontrar null
		}
	    //destruir listas
	}

	subscribirse_a_colas();

	/*
	enviar_mensaje(socket_cola_localized, "localized");
	log_debug(logger,"Mensaje %s enviado, al socket: %d.","localized",socket_cola_localized);	//Logueo envio de mensaje
	sleep(3);

	socket_cola_caught = connect_to(ip,puerto,wait_time);
	enviar_mensaje(socket_cola_caught, "caught");
	log_debug(logger,"Mensaje %s enviado, al socket: %d.","caught",socket_cola_caught);	//Logueo envio de mensaje
	sleep(3);

	socket_cola_appeared = connect_to(ip,puerto,wait_time);
	enviar_mensaje(socket_cola_appeared, "appeared");
	log_debug(logger,"Mensaje %s enviado, al socket: %d.","appeared",socket_cola_appeared);	//Logueo envio de mensaje
	sleep(3);*/
}


int main(void) {

	logger = log_create("team.log","log",1,LOG_LEVEL_DEBUG);
	config = config_create("config");
	inicializar_team();

	return EXIT_SUCCESS;
}
