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



t_entrenador* crear_entrenador(char* posicion, char* pokemones, char* objetivos){
	t_entrenador* entrenador = malloc(sizeof(t_entrenador));

	char **auxiliar = string_split(posicion,"|");

	entrenador->posicion_x = atoi(auxiliar[0]);
	entrenador->posicion_y = atoi(auxiliar[1]);


	auxiliar = string_split(pokemones,"|"); //ultima posicion tiene null

	entrenador->pokemones = list_create();

	while(*auxiliar){
		list_add(entrenador->pokemones,*auxiliar);
		auxiliar++;
	}

	auxiliar = string_split(objetivos,"|"); //ultima posicion tiene null

	entrenador->objetivos = list_create();
	while(*auxiliar){
		list_add(entrenador->objetivos,*auxiliar);
		auxiliar++;
	}

	return entrenador;
}

t_list* obtener_entrenadores(){
	entrenadores = list_create();
	char **lista_de_objetivos, **lista_de_pokemones, **lista_de_posiciones;
	t_entrenador* entrenador;

	lista_de_posiciones = config_get_array_value(config,"POSICIONES_ENTRENADORES");

	lista_de_pokemones = config_get_array_value(config,"POKEMON_ENTRENADORES");

	lista_de_objetivos = config_get_array_value(config,"OBJETIVOS_ENTRENADORES");


	while(*lista_de_posiciones){
		entrenador = crear_entrenador(*lista_de_posiciones, *lista_de_pokemones, *lista_de_objetivos);
		list_add(entrenadores, entrenador);
		lista_de_objetivos++;
		lista_de_pokemones++;
		lista_de_posiciones++;
	}

	return entrenadores;
}

void inicializar_team(){

	int socket_broker, socket_cola_localized, socket_cola_caught, socket_cola_appeared;

	logger = log_create("team.log","log",1,LOG_LEVEL_DEBUG);
	config = config_create("config");


	entrenadores = obtener_entrenadores();

	//Obtiene los datos IP,PUERTO WAIT_TIME desde la config



	ip_broker = config_get_string_value(config,"IP_BROKER");
	log_debug(logger,config_get_string_value(config,"IP_BROKER")); //pido y logueo ip
	puerto_broker = config_get_string_value(config,"PUERTO_BROKER");
	log_debug(logger,config_get_string_value(config,"PUERTO_BROKER")); //pido y logueo puerto
	wait_time = config_get_int_value(config,"TIEMPO_RECONEXION");

	socket_cola_appeared = subscribirse_a_cola(COLA_APPEARED_POKEMON);
	socket_cola_localized = subscribirse_a_cola(COLA_LOCALIZED_POKEMON);
	socket_cola_caught = subscribirse_a_cola(COLA_CAUGHT_POKEMON);





//	enviar_mensajes_get();

}


int main(void) {


	inicializar_team();


	return EXIT_SUCCESS;
}
